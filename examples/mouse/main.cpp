#include <mwtl/mwtl.h>

#include <windowsx.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class MouseWindow final : public mwtl::Window<MouseWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"Mouse demo — move or click in the client area")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    BEGIN_MSG_MAP(MouseWindow)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLeftButtonDown)
        CHAIN_MSG_MAP(mwtl::Window<MouseWindow>)
    END_MSG_MAP()

private:
    void ShowPoint(const wchar_t* event_name, LPARAM position) {
        const int x = GET_X_LPARAM(position);
        const int y = GET_Y_LPARAM(position);
        wchar_t title[128]{};
        _snwprintf_s(title, _countof(title), _TRUNCATE,
                     L"%s at client point (%d, %d)", event_name, x, y);
        SetTitle(title);
    }

    LRESULT OnMouseMove(UINT, WPARAM, LPARAM position, BOOL&) {
        ShowPoint(L"WM_MOUSEMOVE", position);
        return 0;
    }

    LRESULT OnLeftButtonDown(UINT, WPARAM, LPARAM position, BOOL&) {
        ShowPoint(L"WM_LBUTTONDOWN", position);
        return 0;
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<MouseWindow>(show_command); }
    catch (const std::exception& error) { ::OutputDebugStringA(error.what()); }
    catch (...) { ::OutputDebugStringW(L"Unknown exception at wWinMain.\r\n"); }
    return EXIT_FAILURE;
}
