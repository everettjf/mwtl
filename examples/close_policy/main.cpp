#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class ClosePolicyWindow final : public mwtl::Window<ClosePolicyWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"Close policy demo — close twice to exit")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    BEGIN_MSG_MAP(ClosePolicyWindow)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        CHAIN_MSG_MAP(mwtl::Window<ClosePolicyWindow>)
    END_MSG_MAP()

private:
    LRESULT OnClose(UINT, WPARAM, LPARAM, BOOL& handled) {
        if (!close_confirmed_) {
            close_confirmed_ = true;
            SetTitle(L"Close requested once — close again to confirm");
            return 0;
        }
        handled = FALSE;
        return 0;
    }

    bool close_confirmed_ = false;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<ClosePolicyWindow>(show_command); }
    catch (const std::exception& error) { ::OutputDebugStringA(error.what()); }
    catch (...) { ::OutputDebugStringW(L"Unknown exception at wWinMain.\r\n"); }
    return EXIT_FAILURE;
}
