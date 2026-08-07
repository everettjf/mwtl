#include <mwtl/mwtl.h>

#include <cstdlib>

class SystemWindow final : public mwtl::Window<SystemWindow> {
public:
    void BuildUI() { SetTitle(L"Native system lifecycle messages"); }

    BEGIN_MSG_MAP(SystemWindow)
        MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnNotification)
        MESSAGE_HANDLER(WM_SETTINGCHANGE, OnNotification)
        MESSAGE_HANDLER(WM_POWERBROADCAST, OnNotification)
        MESSAGE_HANDLER(WM_IME_STARTCOMPOSITION, OnNotification)
        MESSAGE_HANDLER(WM_IME_COMPOSITION, OnNotification)
        MESSAGE_HANDLER(WM_QUERYENDSESSION, OnQueryEndSession)
        MESSAGE_HANDLER(WM_ENDSESSION, OnNotification)
        MESSAGE_HANDLER(WM_GETOBJECT, OnGetObject)
        CHAIN_MSG_MAP(mwtl::Window<SystemWindow>)
    END_MSG_MAP()

private:
    LRESULT OnNotification(UINT message, WPARAM, LPARAM, BOOL& handled) noexcept {
        wchar_t title[96]{};
        ::swprintf_s(title, L"Native lifecycle message: 0x%04X", message);
        SetTitle(title);
        handled = FALSE;
        return 0;
    }

    LRESULT OnQueryEndSession(UINT, WPARAM, LPARAM, BOOL&) noexcept {
        return TRUE;
    }

    LRESULT OnGetObject(UINT, WPARAM, LPARAM, BOOL& handled) noexcept {
        // A consumer can return UiaReturnRawElementProvider(...) here.
        handled = FALSE;
        return 0;
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try {
        return mwtl::Application(instance, {mwtl::ComApartment::sta})
            .Run<SystemWindow>(show_command);
    } catch (...) { return EXIT_FAILURE; }
}
