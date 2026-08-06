#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class MinMaxWindow final : public mwtl::Window<MinMaxWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"WM_GETMINMAXINFO — minimum 480 × 280 pixels")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    BEGIN_MSG_MAP(MinMaxWindow)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
        CHAIN_MSG_MAP(mwtl::Window<MinMaxWindow>)
    END_MSG_MAP()

private:
    LRESULT OnGetMinMaxInfo(UINT, WPARAM, LPARAM value, BOOL&) {
        auto* info = reinterpret_cast<MINMAXINFO*>(value);
        info->ptMinTrackSize = {480, 280};
        return 0;
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<MinMaxWindow>(show_command); }
    catch (const std::exception& error) { ::OutputDebugStringA(error.what()); }
    catch (...) { ::OutputDebugStringW(L"Unknown exception at wWinMain.\r\n"); }
    return EXIT_FAILURE;
}
