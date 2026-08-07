#pragma once

#include <windows.h>

#include <atlbase.h>
#include <atlapp.h>
#include <atlframe.h>

#include <cstdlib>
#include <exception>
#include <memory>

#include <mwtl/dpi.h>
#include <mwtl/wakeup.h>
#include <mwtl/window_options.h>

namespace mwtl::detail {

void ReportException(
    const wchar_t* stage,
    UINT message,
    const char* description,
    bool show_user) noexcept;
void ReportUnknownException(const wchar_t* stage, UINT message, bool show_user) noexcept;

}  // namespace mwtl::detail

namespace mwtl {

namespace detail {
struct WindowMarker {};
}

template <typename T, typename ClassTraits = DefaultWindowClassTraits>
class Window : public WTL::CFrameWindowImpl<T>, public detail::WindowMarker {
public:
    using Base = WTL::CFrameWindowImpl<T>;

    Window() : wake_state_(std::make_shared<detail::WindowWakeState>()) {}
    ~Window() = default;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    static WTL::CFrameWndClassInfo& GetWndClassInfo() {
        static WTL::CFrameWndClassInfo info = {
            {sizeof(WNDCLASSEXW),
             ClassTraits::GetClassStyle(),
             Window<T, ClassTraits>::StartWindowProc,
             0,
             0,
             nullptr,
             ClassTraits::GetIcon(),
             ClassTraits::GetCursor(),
             ClassTraits::GetBackground(),
             nullptr,
             ClassTraits::GetClassName(),
             ClassTraits::GetSmallIcon()},
            nullptr,
            nullptr,
            nullptr,
            TRUE,
            0,
            L"",
            0};
        return info;
    }

    [[nodiscard]] HWND GetHwnd() const noexcept { return this->m_hWnd; }

    [[nodiscard]] bool IsWindow() const noexcept {
        return this->m_hWnd != nullptr && ::IsWindow(this->m_hWnd) != FALSE;
    }

    bool SetTitle(const wchar_t* title) noexcept {
        return this->m_hWnd != nullptr && ::SetWindowTextW(this->m_hWnd, title) != FALSE;
    }

    [[nodiscard]] DpiContext GetDpiContext() const noexcept {
        return DpiContext::FromWindow(GetHwnd());
    }

    [[nodiscard]] WindowWakeup GetWakeup() const noexcept { return WindowWakeup(wake_state_); }

    void ConfigureWindowOptions(const WindowOptions& options) noexcept {
        apply_suggested_dpi_rect_ = options.apply_suggested_dpi_rect;
    }

    void ApplyNativeResources(const WindowOptions& options) noexcept {
        const HWND window = GetHwnd();
        if (window == nullptr) {
            return;
        }
        if (options.icon != nullptr) {
            ::SendMessageW(window, WM_SETICON, ICON_BIG,
                           reinterpret_cast<LPARAM>(options.icon));
        }
        if (options.small_icon != nullptr) {
            ::SendMessageW(window, WM_SETICON, ICON_SMALL,
                           reinterpret_cast<LPARAM>(options.small_icon));
        }
        if (options.cursor != nullptr) {
            ::SetClassLongPtrW(
                window, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(options.cursor));
        }
        if (options.background != nullptr) {
            ::SetClassLongPtrW(
                window, GCLP_HBRBACKGROUND,
                reinterpret_cast<LONG_PTR>(options.background));
        }
    }

protected:
    WNDPROC GetWindowProc() override { return &SafeWindowProc; }

private:
    static const wchar_t* StageFor(UINT message, bool creation_complete) noexcept {
        if (!creation_complete) {
            return L"window creation";
        }
        if (message == WM_DESTROY || message == WM_NCDESTROY) {
            return L"window destruction";
        }
        return L"window message dispatch";
    }

    static LRESULT CALLBACK SafeWindowProc(
        HWND object_pointer,
        UINT message,
        WPARAM wparam,
        LPARAM lparam) noexcept {
        auto* self = reinterpret_cast<Window<T, ClassTraits>*>(object_pointer);
        const bool creation_message = !self->creation_complete_;
        const wchar_t* const stage = StageFor(message, self->creation_complete_);
        const _ATL_MSG* const previous_message = self->m_pCurrentMsg;

        try {
            if (message == WindowWakeup::Message() &&
                wparam != reinterpret_cast<WPARAM>(self->wake_state_.get())) {
                return 0;
            }

            if (message == WM_CLOSE && self->recovery_requested_) {
                const HWND window = self->m_hWnd;
                return window != nullptr && ::DestroyWindow(window) != FALSE ? 0 : -1;
            }

            if (message == WM_CREATE) {
                self->wake_state_->window.store(self->m_hWnd, std::memory_order_release);
                static_cast<T*>(self)->BuildUI();
            }

            if (message == WM_DPICHANGED && self->apply_suggested_dpi_rect_ &&
                lparam != 0) {
                const RECT* suggested = reinterpret_cast<const RECT*>(lparam);
                ::SetWindowPos(
                    self->m_hWnd,
                    nullptr,
                    suggested->left,
                    suggested->top,
                    suggested->right - suggested->left,
                    suggested->bottom - suggested->top,
                    SWP_NOZORDER | SWP_NOACTIVATE);
            }

            if (message == WM_NCDESTROY) {
                self->wake_state_->window.store(nullptr, std::memory_order_release);
            }

            const LRESULT result = Base::WindowProc(object_pointer, message, wparam, lparam);

            if (message == WM_CREATE && result != -1) {
                self->creation_complete_ = true;
            }
            if (message == WM_DESTROY) {
                ::PostQuitMessage(self->exit_code_);
            }
            if (message == WM_NCDESTROY) {
                self->wake_state_->window.store(nullptr, std::memory_order_release);
            }
            return result;
        } catch (const std::exception& error) {
            detail::ReportException(stage, message, error.what(), creation_message);
        } catch (...) {
            detail::ReportUnknownException(stage, message, creation_message);
        }

        self->m_pCurrentMsg = previous_message;
        return self->RecoverFromDispatchFailure(message, wparam, lparam, creation_message);
    }

    LRESULT RecoverFromDispatchFailure(
        UINT message,
        WPARAM wparam,
        LPARAM lparam,
        bool creation_message) noexcept {
        exit_code_ = EXIT_FAILURE;
        ::SetLastError(ERROR_UNHANDLED_EXCEPTION);

        if (creation_message) {
            return message == WM_CREATE ? -1 : 0;
        }

        if (message == WM_NCDESTROY) {
            wake_state_->window.store(nullptr, std::memory_order_release);
            const HWND window = this->m_hWnd;
            const LRESULT result = window != nullptr
                ? ::DefWindowProcW(window, message, wparam, lparam)
                : 0;
            this->m_hWnd = nullptr;
            ::PostQuitMessage(exit_code_);
            return result;
        }

        if (message == WM_DESTROY) {
            ::PostQuitMessage(exit_code_);
            return 0;
        }

        recovery_requested_ = true;
        if (this->m_hWnd == nullptr ||
            ::PostMessageW(this->m_hWnd, WM_CLOSE, 0, 0) == FALSE) {
            ::PostQuitMessage(exit_code_);
        }
        return 0;
    }

    bool creation_complete_ = false;
    bool recovery_requested_ = false;
    bool apply_suggested_dpi_rect_ = true;
    int exit_code_ = EXIT_SUCCESS;
    std::shared_ptr<detail::WindowWakeState> wake_state_;
};

}  // namespace mwtl
