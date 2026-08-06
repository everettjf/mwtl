#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class ApplicationWindow final : public mwtl::Window<ApplicationWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"mwtl::Application demo")) {
            throw std::runtime_error("SetTitle failed");
        }
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try {
        mwtl::Application application(instance);
        if (application.GetInstance() != instance) {
            return EXIT_FAILURE;
        }
        return application.Run<ApplicationWindow>(show_command);
    } catch (const std::exception& error) {
        ::OutputDebugStringA(error.what());
    } catch (...) {
        ::OutputDebugStringW(L"Unknown exception at the wWinMain boundary.\r\n");
    }
    return EXIT_FAILURE;
}
