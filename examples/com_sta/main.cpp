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
    return mwtl::RunApplication<ComWindow>(
        instance,
        show_command,
        {},
        {.com_apartment = mwtl::ComApartment::sta});
}
