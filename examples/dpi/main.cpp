#include <mwtl/mwtl.h>

#include <cstdlib>
#include <cwchar>

class DpiWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override { UpdateTitle(GetDpiContext()); }

    mwtl::EventResult OnDpiChanged(const mwtl::DpiChangedEvent& event) noexcept override {
        UpdateTitle(mwtl::DpiContext::FromDpi(event.dpi_x));
        return mwtl::EventResult::Propagate();
    }

private:
    void UpdateTitle(mwtl::DpiContext dpi) noexcept {
        wchar_t title[96]{};
        ::swprintf_s(title, L"DPI demo - %u DPI (%.2fx)", dpi.GetDpi(), dpi.GetScale());
        SetTitle(title);
    }

};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<DpiWindow>(instance, show_command);
}
