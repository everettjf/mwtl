#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class KeyboardWindow final : public mwtl::Window<KeyboardWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"Keyboard demo — press a key; Escape closes")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    BEGIN_MSG_MAP(KeyboardWindow)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        CHAIN_MSG_MAP(mwtl::Window<KeyboardWindow>)
    END_MSG_MAP()

private:
    LRESULT OnKeyDown(UINT, WPARAM key, LPARAM, BOOL& handled) {
        if (key == VK_ESCAPE) {
            ::SendMessageW(GetHwnd(), WM_CLOSE, 0, 0);
            return 0;
        }
        wchar_t title[96]{};
        _snwprintf_s(title, _countof(title), _TRUNCATE,
                     L"WM_KEYDOWN virtual key: 0x%02llX",
                     static_cast<unsigned long long>(key));
        SetTitle(title);
        handled = TRUE;
        return 0;
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<KeyboardWindow>(show_command); }
    catch (const std::exception& error) { ::OutputDebugStringA(error.what()); }
    catch (...) { ::OutputDebugStringW(L"Unknown exception at wWinMain.\r\n"); }
    return EXIT_FAILURE;
}
