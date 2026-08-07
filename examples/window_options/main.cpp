#include <mwtl/mwtl.h>

#include <cstdlib>

using mwtl::operator""_dip;

struct DemoClassTraits {
    static const wchar_t* GetClassName() noexcept { return L"mwtl.WindowOptionsDemo"; }
    static UINT GetClassStyle() noexcept { return CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; }
    static HICON GetIcon() noexcept { return nullptr; }
    static HICON GetSmallIcon() noexcept { return nullptr; }
    static HCURSOR GetCursor() noexcept { return ::LoadCursorW(nullptr, IDC_ARROW); }
    static HBRUSH GetBackground() noexcept { return reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1); }
};

class OptionsWindow final : public mwtl::Window<OptionsWindow, DemoClassTraits> {
public:
    void BuildUI() { SetTitle(L"Configurable class, style and DIP client size"); }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try {
        mwtl::WindowOptions options;
        options.title = L"Window options demo";
        options.use_default_bounds = false;
        options.initial_bounds = {{0.0_dip, 0.0_dip}, {800.0_dip, 500.0_dip}};
        options.bounds_are_client_size = true;
        options.center_in_work_area = true;
        return mwtl::Application(instance).Run<OptionsWindow>(show_command, options);
    } catch (...) { return EXIT_FAILURE; }
}
