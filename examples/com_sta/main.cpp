#include <mwtl/mwtl.h>

#include <cstdlib>
#include <objbase.h>

class ComWindow final : public mwtl::Window<ComWindow> {
public:
    void BuildUI() {
        APTTYPE type{};
        APTTYPEQUALIFIER qualifier{};
        const bool sta = SUCCEEDED(::CoGetApartmentType(&type, &qualifier)) &&
            (type == APTTYPE_STA || type == APTTYPE_MAINSTA);
        SetTitle(sta ? L"COM STA initialized by mwtl::Application"
                     : L"Unexpected COM apartment");
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try {
        const mwtl::ApplicationOptions options{mwtl::ComApartment::sta};
        return mwtl::Application(instance, options).Run<ComWindow>(show_command);
    } catch (...) { return EXIT_FAILURE; }
}
