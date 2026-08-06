#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class MainWindow final : public mwtl::Window<MainWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"mwtl hello")) {
            throw std::runtime_error("SetWindowTextW failed while building the main window");
        }
    }
};

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE,
    PWSTR,
    int show_command) {
    try {
        return mwtl::Application(instance).Run<MainWindow>(show_command);
    } catch (const std::exception& error) {
        ::OutputDebugStringA("mwtl: std::exception escaped the application boundary: ");
        ::OutputDebugStringA(error.what());
        ::OutputDebugStringA("\r\n");
    } catch (...) {
        ::OutputDebugStringW(L"mwtl: unknown exception escaped the application boundary.\r\n");
    }
    ::MessageBoxW(
        nullptr,
        L"mwtl could not start because of an unexpected error.",
        L"mwtl startup error",
        MB_OK | MB_ICONERROR | MB_TASKMODAL);
    return EXIT_FAILURE;
}
