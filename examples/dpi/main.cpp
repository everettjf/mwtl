#include <mwtl/mwtl.h>

#include <cstdlib>
#include <cwchar>

class DpiWindow final : public mwtl::Window<DpiWindow> {
public:
    void BuildUI() { UpdateTitle(GetDpiContext()); }

    BEGIN_MSG_MAP(DpiWindow)
        MESSAGE_HANDLER(WM_DPICHANGED, OnDpiChanged)
        CHAIN_MSG_MAP(mwtl::Window<DpiWindow>)
    END_MSG_MAP()

private:
    void UpdateTitle(mwtl::DpiContext dpi) noexcept {
        wchar_t title[96]{};
        ::swprintf_s(title, L"DPI demo - %u DPI (%.2fx)", dpi.GetDpi(), dpi.GetScale());
        SetTitle(title);
    }

    LRESULT OnDpiChanged(UINT, WPARAM dpi, LPARAM, BOOL& handled) noexcept {
        UpdateTitle(mwtl::DpiContext::FromDpi(LOWORD(dpi)));
        handled = FALSE; // Window<T> applies the suggested rectangle.
        return 0;
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<DpiWindow>(show_command); }
    catch (...) { return EXIT_FAILURE; }
}
