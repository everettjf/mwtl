#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class ResizeWindow final : public mwtl::Window<ResizeWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"Resize demo — resize this window")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    BEGIN_MSG_MAP(ResizeWindow)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        CHAIN_MSG_MAP(mwtl::Window<ResizeWindow>)
    END_MSG_MAP()

private:
    LRESULT OnSize(UINT, WPARAM state, LPARAM size, BOOL& handled) {
        wchar_t title[128]{};
        _snwprintf_s(title, _countof(title), _TRUNCATE,
                     L"WM_SIZE: %u × %u (state %llu)",
                     LOWORD(size), HIWORD(size),
                     static_cast<unsigned long long>(state));
        SetTitle(title);
        handled = FALSE;
        return 0;
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<ResizeWindow>(show_command); }
    catch (const std::exception& error) { ::OutputDebugStringA(error.what()); }
    catch (...) { ::OutputDebugStringW(L"Unknown exception at wWinMain.\r\n"); }
    return EXIT_FAILURE;
}
