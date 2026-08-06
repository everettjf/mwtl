#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class WindowStateWindow final : public mwtl::Window<WindowStateWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"Window state demo — minimize or maximize")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    BEGIN_MSG_MAP(WindowStateWindow)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        CHAIN_MSG_MAP(mwtl::Window<WindowStateWindow>)
    END_MSG_MAP()

private:
    LRESULT OnSize(UINT, WPARAM state, LPARAM, BOOL& handled) {
        const wchar_t* label = L"restored";
        if (state == SIZE_MINIMIZED) label = L"minimized";
        if (state == SIZE_MAXIMIZED) label = L"maximized";
        wchar_t title[96]{};
        _snwprintf_s(title, _countof(title), _TRUNCATE,
                     L"Window state: %s", label);
        SetTitle(title);
        handled = FALSE;
        return 0;
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<WindowStateWindow>(show_command); }
    catch (const std::exception& error) { ::OutputDebugStringA(error.what()); }
    catch (...) { ::OutputDebugStringW(L"Unknown exception at wWinMain.\r\n"); }
    return EXIT_FAILURE;
}
