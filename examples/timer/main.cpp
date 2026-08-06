#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class TimerWindow final : public mwtl::Window<TimerWindow> {
public:
    void BuildUI() {
        ::SetWindowPos(GetHwnd(), nullptr, 0, 0, 900, 560,
                       SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        if (!SetTitle(L"Timer demo — waiting for WM_TIMER")) {
            throw std::runtime_error("SetTitle failed");
        }
        if (::SetTimer(GetHwnd(), 1, 1000, nullptr) == 0) {
            throw std::runtime_error("SetTimer failed");
        }
    }

    BEGIN_MSG_MAP(TimerWindow)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        CHAIN_MSG_MAP(mwtl::Window<TimerWindow>)
    END_MSG_MAP()

private:
    LRESULT OnTimer(UINT, WPARAM timer_id, LPARAM, BOOL&) {
        wchar_t title[96]{};
        _snwprintf_s(title, _countof(title), _TRUNCATE,
                     L"WM_TIMER tick %u", ++ticks_);
        SetTitle(title);
        return timer_id == 1 ? 0 : 1;
    }

    LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& handled) {
        ::KillTimer(GetHwnd(), 1);
        handled = FALSE;
        return 0;
    }

    unsigned int ticks_ = 0;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<TimerWindow>(show_command); }
    catch (const std::exception& error) { ::OutputDebugStringA(error.what()); }
    catch (...) { ::OutputDebugStringW(L"Unknown exception at wWinMain.\r\n"); }
    return EXIT_FAILURE;
}
