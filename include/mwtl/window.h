#pragma once

#include <windows.h>

#include <atlbase.h>
#include <atlapp.h>
#include <atlframe.h>

#include <cstdlib>
#include <exception>

namespace mwtl::detail {

void ReportException(
    const wchar_t* stage,
    UINT message,
    const char* description,
    bool show_user) noexcept;
void ReportUnknownException(const wchar_t* stage, UINT message, bool show_user) noexcept;

}  // namespace mwtl::detail

namespace mwtl {

template <typename T>
class Window : public WTL::CFrameWindowImpl<T> {
public:
    using Base = WTL::CFrameWindowImpl<T>;

    Window() noexcept = default;
    ~Window() = default;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    DECLARE_FRAME_WND_CLASS_EX2(
        L"mwtl.MainWindow",
        Window<T>,
        0,
        CS_HREDRAW | CS_VREDRAW,
        COLOR_WINDOW)

    HWND GetHwnd() const noexcept { return this->m_hWnd; }

    bool IsWindow() const noexcept {
        return this->m_hWnd != nullptr && ::IsWindow(this->m_hWnd) != FALSE;
    }

    bool SetTitle(const wchar_t* title) noexcept {
        return this->m_hWnd != nullptr && ::SetWindowTextW(this->m_hWnd, title) != FALSE;
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
        auto* self = reinterpret_cast<Window<T>*>(object_pointer);
        const bool creation_message = !self->creation_complete_;
        const wchar_t* const stage = StageFor(message, self->creation_complete_);
        const _ATL_MSG* const previous_message = self->m_pCurrentMsg;

        try {
            if (message == WM_CLOSE && self->recovery_requested_) {
                const HWND window = self->m_hWnd;
                return window != nullptr && ::DestroyWindow(window) != FALSE ? 0 : -1;
            }

            if (message == WM_CREATE) {
                static_cast<T*>(self)->BuildUI();
            }

            const LRESULT result = Base::WindowProc(object_pointer, message, wparam, lparam);

            if (message == WM_CREATE && result != -1) {
                self->creation_complete_ = true;
            }
            if (message == WM_DESTROY) {
                ::PostQuitMessage(self->exit_code_);
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
    int exit_code_ = EXIT_SUCCESS;
};

}  // namespace mwtl
