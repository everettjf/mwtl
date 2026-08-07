#pragma once

#include <windows.h>

#include <atlbase.h>
#include <atlapp.h>
#include <wil/resource.h>

#include <cstdlib>
#include <concepts>
#include <exception>

#include <mwtl/message_pump.h>
#include <mwtl/window.h>
#include <mwtl/window_options.h>

namespace mwtl {

template <typename T>
concept MainWindow = std::derived_from<T, detail::WindowMarker> &&
    std::default_initializable<T> && requires(T& window) {
        { window.BuildUI() } -> std::same_as<void>;
    };

enum class ComApartment {
    none,
    sta,
    mta,
};

struct ApplicationOptions {
    ComApartment com_apartment = ComApartment::none;
};

class Application final {
public:
    explicit Application(HINSTANCE instance) noexcept;
    Application(HINSTANCE instance, ApplicationOptions options) noexcept;
    ~Application() noexcept;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    template <MainWindow MainWindowType>
    int Run(int show_command) noexcept {
        return RunImpl<MainWindowType>(show_command, WindowOptions{}, nullptr);
    }

    template <MainWindow MainWindowType>
    int Run(int show_command, const WindowOptions& options) noexcept {
        return RunImpl<MainWindowType>(show_command, options, nullptr);
    }

    template <MainWindow MainWindowType>
    int Run(
        int show_command,
        const WindowOptions& options,
        MessagePump& message_pump) noexcept {
        return RunImpl<MainWindowType>(show_command, options, &message_pump);
    }

    [[nodiscard]] HINSTANCE GetInstance() const noexcept { return instance_; }

private:
    template <MainWindow MainWindowType>
    int RunImpl(
        int show_command,
        const WindowOptions& options,
        MessagePump* message_pump) noexcept {
        if (!BeginRun()) {
            return EXIT_FAILURE;
        }

        auto cleanup = wil::scope_exit([this]() noexcept { EndRun(); });

        try {
            MainWindowType main_window;
            main_window.ConfigureWindowOptions(options);
            RECT bounds = options.use_default_bounds
                ? ATL::CWindow::rcDefault
                : detail::ResolveWindowBounds(options);
            const HWND window = main_window.Create(
                nullptr,
                bounds,
                options.title,
                options.style,
                options.extended_style);
            if (window == nullptr) {
                ReportWindowCreationFailure(::GetLastError());
                return EXIT_FAILURE;
            }

            main_window.ApplyNativeResources(options);

            ::ShowWindow(window, show_command);
            ::SetLastError(ERROR_SUCCESS);
            if (!::UpdateWindow(window)) {
                const DWORD error = ::GetLastError();
                if (error != ERROR_SUCCESS) {
                    ReportWindowDisplayFailure(error);
                    ::DestroyWindow(window);
                    return EXIT_FAILURE;
                }
            }

            const int loop_result = RunMessageLoop(message_pump);

            // A custom pump may stop because one of its callbacks failed before
            // the window received WM_CLOSE. Keep the stack-owned C++ window
            // object alive until its HWND is synchronously detached.
            if (main_window.IsWindow()) {
                const HWND hwnd = main_window.GetHwnd();
                if (::DestroyWindow(hwnd) == FALSE) {
                    ReportWindowDisplayFailure(::GetLastError());
                    return EXIT_FAILURE;
                }
            }

            return loop_result;
        } catch (const std::exception& error) {
            ReportRunException(error.what());
        } catch (...) {
            ReportUnknownRunException();
        }
        return EXIT_FAILURE;
    }

    bool BeginRun() noexcept;
    void EndRun() noexcept;
    int RunMessageLoop(MessagePump* message_pump);
    void ReportWindowCreationFailure(DWORD error) noexcept;
    void ReportWindowDisplayFailure(DWORD error) noexcept;
    void ReportRunException(const char* description) noexcept;
    void ReportUnknownRunException() noexcept;

    HINSTANCE instance_ = nullptr;  // Non-owning process module handle.
    ApplicationOptions options_{};
    WTL::CMessageLoop message_loop_;
    wil::unique_couninitialize_call com_uninitialize_;
    bool module_initialized_ = false;
    bool loop_registered_ = false;
    bool running_ = false;
};

template <MainWindow MainWindowType>
[[nodiscard]] int RunApplication(
    HINSTANCE instance,
    int show_command,
    const WindowOptions& window_options = {},
    ApplicationOptions application_options = {}) noexcept {
    return Application(instance, application_options)
        .Run<MainWindowType>(show_command, window_options);
}

template <MainWindow MainWindowType>
[[nodiscard]] int RunApplication(
    HINSTANCE instance,
    int show_command,
    MessagePump& message_pump,
    const WindowOptions& window_options = {},
    ApplicationOptions application_options = {}) noexcept {
    return Application(instance, application_options)
        .Run<MainWindowType>(show_command, window_options, message_pump);
}

}  // namespace mwtl
