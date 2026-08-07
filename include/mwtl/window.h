#pragma once

#include <windows.h>

#include <atlbase.h>
#include <atlapp.h>
#include <atlframe.h>

#include <cstdlib>
#include <concepts>
#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <mwtl/dpi.h>
#include <mwtl/events.h>
#include <mwtl/layout.h>
#include <mwtl/wakeup.h>
#include <mwtl/window_options.h>

extern WTL::CAppModule _Module;

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
    class AcceleratorFilter final : public WTL::CMessageFilter {
    public:
        explicit AcceleratorFilter(Window* owner) noexcept : owner_(owner) {}
        BOOL PreTranslateMessage(MSG* message) override {
            return message != nullptr && owner_->m_hWnd != nullptr &&
                    owner_->m_hAccel != nullptr
                ? ::TranslateAcceleratorW(owner_->m_hWnd, owner_->m_hAccel, message)
                : FALSE;
        }

    private:
        Window* owner_;
    };

public:
    using Base = WTL::CFrameWindowImpl<T>;

    Window()
        : accelerator_filter_(this),
          wake_state_(std::make_shared<detail::WindowWakeState>()) {}
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

    HWND GetHwnd() const noexcept { return this->m_hWnd; }

    bool IsWindow() const noexcept {
        return this->m_hWnd != nullptr && ::IsWindow(this->m_hWnd) != FALSE;
    }

    bool SetTitle(const wchar_t* title) noexcept {
        return this->m_hWnd != nullptr && ::SetWindowTextW(this->m_hWnd, title) != FALSE;
    }

    bool SetTitle(std::wstring_view title) {
        const std::wstring terminated{title};
        return SetTitle(terminated.c_str());
    }

    LRESULT Close() noexcept {
        return this->m_hWnd != nullptr
            ? ::SendMessageW(this->m_hWnd, WM_CLOSE, 0, 0)
            : 0;
    }

    DpiContext GetDpiContext() const noexcept {
        return DpiContext::FromWindow(GetHwnd());
    }

    WindowWakeup GetWakeup() const noexcept { return WindowWakeup(wake_state_); }

    void SetAccelerators(HACCEL accelerators) noexcept {
        this->m_hAccel = accelerators;  // Non-owning; the table must outlive the window.
        WTL::CMessageLoop* loop = _Module.GetMessageLoop();
        if (loop == nullptr) return;
        if (accelerators != nullptr && !accelerator_filter_registered_) {
            accelerator_filter_registered_ =
                loop->AddMessageFilter(&accelerator_filter_) != FALSE;
        } else if (accelerators == nullptr && accelerator_filter_registered_) {
            loop->RemoveMessageFilter(&accelerator_filter_);
            accelerator_filter_registered_ = false;
        }
    }

    bool SetLayout(LayoutHost& layout) {
        owned_layout_.reset();
        layout_ = &layout;  // Non-owning; the layout must outlive the HWND.
        return !IsWindow() || layout.Arrange(GetHwnd());
    }

    void UseLayout(LayoutNode root) {
        owned_layout_.emplace();
        owned_layout_->SetRoot(std::move(root));
        layout_ = &*owned_layout_;
        if (IsWindow() && !layout_->Arrange(GetHwnd())) {
            layout_ = nullptr;
            owned_layout_.reset();
            throw std::runtime_error("mwtl could not arrange the owned layout");
        }
    }

    void ClearLayout() noexcept {
        layout_ = nullptr;
        owned_layout_.reset();
    }

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

    BOOL ProcessWindowMessage(
        HWND window,
        UINT message,
        WPARAM wparam,
        LPARAM lparam,
        LRESULT& result,
        DWORD message_map_id = 0) override {
        if (message_map_id == 0 &&
            DispatchModernMessage(message, wparam, lparam, result)) {
            return TRUE;
        }
        return Base::ProcessWindowMessage(
            window, message, wparam, lparam, result, message_map_id);
    }

protected:
    WNDPROC GetWindowProc() override { return &SafeWindowProc; }

private:
    template <typename Callback>
    static bool InvokeModernHandler(Callback&& callback, LRESULT& result) {
        using Return = std::invoke_result_t<Callback>;
        if constexpr (std::same_as<Return, void>) {
            std::invoke(std::forward<Callback>(callback));
            result = 0;
            return true;
        } else {
            static_assert(
                std::same_as<Return, EventResult>,
                "mwtl convention handlers must return void or mwtl::EventResult");
            const EventResult reply =
                std::invoke(std::forward<Callback>(callback));
            result = reply.result;
            return reply.handled;
        }
    }

    bool DispatchModernMessage(
        UINT message,
        WPARAM wparam,
        LPARAM lparam,
        LRESULT& result) {
        T& target = *static_cast<T*>(this);

        if (message == WM_CLOSE) {
            if constexpr (requires(T& value) { value.OnClose(); }) {
                return InvokeModernHandler(
                    [&target] { return target.OnClose(); }, result);
            }
        }
        if (message == WM_KEYDOWN) {
            if constexpr (requires(T& value, const KeyEvent& event) {
                              value.OnKeyDown(event);
                          }) {
                const KeyEvent event{
                    .virtual_key = static_cast<UINT>(wparam),
                    .repeat_count = static_cast<std::uint16_t>(lparam & 0xFFFF),
                    .scan_code = static_cast<std::uint8_t>((lparam >> 16) & 0xFF),
                    .extended = (lparam & (1LL << 24)) != 0,
                    .was_down = (lparam & (1LL << 30)) != 0,
                };
                return InvokeModernHandler(
                    [&target, &event] { return target.OnKeyDown(event); }, result);
            }
        }
        if (message == WM_MOUSEMOVE || message == WM_LBUTTONDOWN) {
            const MouseEvent event{
                .position = POINT{
                    static_cast<short>(LOWORD(lparam)),
                    static_cast<short>(HIWORD(lparam)),
                },
                .key_state = wparam,
            };
            if (message == WM_MOUSEMOVE) {
                if constexpr (requires(T& value, const MouseEvent& input) {
                                  value.OnMouseMove(input);
                              }) {
                    return InvokeModernHandler(
                        [&target, &event] { return target.OnMouseMove(event); },
                        result);
                }
            } else {
                if constexpr (requires(T& value, const MouseEvent& input) {
                                  value.OnLeftButtonDown(input);
                              }) {
                    return InvokeModernHandler(
                        [&target, &event] {
                            return target.OnLeftButtonDown(event);
                        },
                        result);
                }
            }
        }
        if (message == WM_HSCROLL || message == WM_VSCROLL) {
            if constexpr (requires(T& value, const ScrollEvent& event) {
                              value.OnScroll(event);
                          }) {
                const ScrollEvent event{
                    .control = reinterpret_cast<HWND>(lparam),
                    .request = LOWORD(wparam),
                    .position = HIWORD(wparam),
                    .horizontal = message == WM_HSCROLL,
                };
                return InvokeModernHandler(
                    [&target, &event] { return target.OnScroll(event); },
                    result);
            }
        }
        if (message == WM_SIZE) {
            if (layout_ != nullptr && wparam != SIZE_MINIMIZED) {
                static_cast<void>(layout_->Arrange(GetHwnd()));
            }
            if constexpr (requires(T& value, const ResizeEvent& event) {
                              value.OnResize(event);
                          }) {
                const ResizeEvent event{
                    .client_size = SIZE{
                        static_cast<LONG>(LOWORD(lparam)),
                        static_cast<LONG>(HIWORD(lparam)),
                    },
                    .state = detail::DecodeWindowSizeState(wparam),
                };
                return InvokeModernHandler(
                    [&target, &event] { return target.OnResize(event); }, result);
            }
        }
        if (message == WM_TIMER) {
            if constexpr (requires(T& value, TimerId id) { value.OnTimer(id); }) {
                return InvokeModernHandler(
                    [&target, wparam] { return target.OnTimer(TimerId{wparam}); },
                    result);
            }
        }
        if (message == WM_DPICHANGED) {
            if constexpr (requires(T& value, const DpiChangedEvent& event) {
                              value.OnDpiChanged(event);
                          }) {
                const RECT empty{};
                const RECT& suggested = lparam != 0
                    ? *reinterpret_cast<const RECT*>(lparam)
                    : empty;
                const DpiChangedEvent event{
                    .dpi_x = LOWORD(wparam),
                    .dpi_y = HIWORD(wparam),
                    .suggested_bounds = suggested,
                };
                return InvokeModernHandler(
                    [&target, &event] { return target.OnDpiChanged(event); },
                    result);
            }
        }
        if (message == WM_COMMAND) {
            if constexpr (requires(T& value, const CommandEvent& event) {
                              value.OnCommand(event);
                          }) {
                const CommandEvent event{
                    .id = ControlId{LOWORD(wparam)},
                    .notification = HIWORD(wparam),
                    .control = reinterpret_cast<HWND>(lparam),
                };
                return InvokeModernHandler(
                    [&target, &event] { return target.OnCommand(event); }, result);
            }
        }
        if (message == WM_NOTIFY && lparam != 0) {
            if constexpr (requires(T& value, const NotifyEvent& event) {
                              value.OnNotify(event);
                          }) {
                const NotifyEvent event{*reinterpret_cast<const NMHDR*>(lparam)};
                return InvokeModernHandler(
                    [&target, &event] { return target.OnNotify(event); }, result);
            }
        }
        if (message == WM_GETMINMAXINFO && lparam != 0) {
            if constexpr (requires(T& value, MinMaxInfoEvent event) {
                              value.OnMinMaxInfo(event);
                          }) {
                MinMaxInfoEvent event{
                    *reinterpret_cast<MINMAXINFO*>(lparam)};
                return InvokeModernHandler(
                    [&target, &event] { return target.OnMinMaxInfo(event); },
                    result);
            }
        }
        if (message == WM_PAINT) {
            if constexpr (requires(T& value, PaintEvent& event) {
                              value.OnPaint(event);
                          }) {
                PaintEvent event{GetHwnd()};
                return InvokeModernHandler(
                    [&target, &event] { return target.OnPaint(event); }, result);
            }
        }
        if (message == WindowWakeup::Message()) {
            if constexpr (requires(T& value) { value.OnWakeup(); }) {
                return InvokeModernHandler(
                    [&target] { return target.OnWakeup(); }, result);
            }
        }
        if (message != WM_CREATE && message != WM_DESTROY &&
            message != WM_NCDESTROY) {
            if constexpr (requires(T& value, const WindowMessage& event) {
                              value.OnMessage(event);
                          }) {
                const WindowMessage event{message, wparam, lparam};
                return InvokeModernHandler(
                    [&target, &event] { return target.OnMessage(event); },
                    result);
            }
        }
        return false;
    }

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
                if (self->accelerator_filter_registered_) {
                    if (WTL::CMessageLoop* loop = _Module.GetMessageLoop(); loop != nullptr) {
                        loop->RemoveMessageFilter(&self->accelerator_filter_);
                    }
                    self->accelerator_filter_registered_ = false;
                }
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

    AcceleratorFilter accelerator_filter_;
    bool creation_complete_ = false;
    bool accelerator_filter_registered_ = false;
    bool recovery_requested_ = false;
    bool apply_suggested_dpi_rect_ = true;
    std::optional<LayoutHost> owned_layout_;
    LayoutHost* layout_ = nullptr;  // Non-owning.
    int exit_code_ = EXIT_SUCCESS;
    std::shared_ptr<detail::WindowWakeState> wake_state_;
};

// Concise inheritance for ordinary applications. The WTL-required CRTP type is
// fixed here; use Window<T> directly for compile-time dispatch or WTL maps.
class WindowBase : public Window<WindowBase> {
public:
    virtual ~WindowBase() = default;

    virtual void BuildUI() = 0;

    virtual EventResult OnClose() { return EventResult::Propagate(); }
    virtual EventResult OnKeyDown(const KeyEvent&) { return EventResult::Propagate(); }
    virtual EventResult OnMouseMove(const MouseEvent&) { return EventResult::Propagate(); }
    virtual EventResult OnLeftButtonDown(const MouseEvent&) { return EventResult::Propagate(); }
    virtual EventResult OnScroll(const ScrollEvent&) { return EventResult::Propagate(); }
    virtual EventResult OnResize(const ResizeEvent&) { return EventResult::Propagate(); }
    virtual EventResult OnTimer(TimerId) { return EventResult::Propagate(); }
    virtual EventResult OnDpiChanged(const DpiChangedEvent&) { return EventResult::Propagate(); }
    virtual EventResult OnCommand(const CommandEvent&) { return EventResult::Propagate(); }
    virtual EventResult OnNotify(const NotifyEvent&) { return EventResult::Propagate(); }
    virtual EventResult OnMinMaxInfo(MinMaxInfoEvent) { return EventResult::Propagate(); }
    virtual EventResult OnPaint(PaintEvent&) { return EventResult::Propagate(); }
    virtual EventResult OnWakeup() { return EventResult::Propagate(); }
    virtual EventResult OnMessage(const WindowMessage&) { return EventResult::Propagate(); }
};

}  // namespace mwtl
