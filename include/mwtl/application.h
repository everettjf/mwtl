#pragma once

#include <windows.h>

#include <atlbase.h>
#include <atlapp.h>
#include <wil/resource.h>

#include <cstdlib>
#include <exception>
#include <type_traits>

#include <mwtl/window.h>

namespace mwtl {

class Application final {
public:
    explicit Application(HINSTANCE instance) noexcept;
    ~Application() noexcept;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    template <typename MainWindow>
    int Run(int show_command) noexcept {
        static_assert(std::is_base_of_v<Window<MainWindow>, MainWindow>,
                      "MainWindow must derive from mwtl::Window<MainWindow>");

        if (!BeginRun()) {
            return EXIT_FAILURE;
        }

        auto cleanup = wil::scope_exit([this]() noexcept { EndRun(); });

        try {
            MainWindow main_window;
            const HWND window = main_window.Create(
                nullptr,
                ATL::CWindow::rcDefault,
                L"mwtl application");
            if (window == nullptr) {
                ReportWindowCreationFailure(::GetLastError());
                return EXIT_FAILURE;
            }

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

            return RunMessageLoop();
        } catch (const std::exception& error) {
            ReportRunException(error.what());
        } catch (...) {
            ReportUnknownRunException();
        }
        return EXIT_FAILURE;
    }

    HINSTANCE GetInstance() const noexcept { return instance_; }

private:
    bool BeginRun() noexcept;
    void EndRun() noexcept;
    int RunMessageLoop();
    void ReportWindowCreationFailure(DWORD error) noexcept;
    void ReportWindowDisplayFailure(DWORD error) noexcept;
    void ReportRunException(const char* description) noexcept;
    void ReportUnknownRunException() noexcept;

    HINSTANCE instance_ = nullptr;  // Non-owning process module handle.
    WTL::CMessageLoop message_loop_;
    bool module_initialized_ = false;
    bool loop_registered_ = false;
    bool running_ = false;
};

}  // namespace mwtl
