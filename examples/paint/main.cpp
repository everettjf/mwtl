#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class PaintWindow final : public mwtl::Window<PaintWindow> {
public:
    void BuildUI() {
        ::SetWindowPos(GetHwnd(), nullptr, 0, 0, 900, 560,
                       SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        if (!SetTitle(L"Native GDI paint demo")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    BEGIN_MSG_MAP(PaintWindow)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        CHAIN_MSG_MAP(mwtl::Window<PaintWindow>)
    END_MSG_MAP()

private:
    LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL&) {
        PAINTSTRUCT paint{};
        const HDC dc = ::BeginPaint(GetHwnd(), &paint);
        if (dc != nullptr) {
            RECT client{};
            ::GetClientRect(GetHwnd(), &client);
            ::SetBkMode(dc, TRANSPARENT);
            ::SetTextColor(dc, RGB(24, 78, 119));
            ::DrawTextW(dc,
                        L"Native HWND + WM_PAINT",
                        -1, &client,
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            ::EndPaint(GetHwnd(), &paint);
        }
        return 0;
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<PaintWindow>(show_command); }
    catch (const std::exception& error) { ::OutputDebugStringA(error.what()); }
    catch (...) { ::OutputDebugStringW(L"Unknown exception at wWinMain.\r\n"); }
    return EXIT_FAILURE;
}
