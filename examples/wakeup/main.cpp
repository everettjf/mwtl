#include <mwtl/mwtl.h>

#include <cstdlib>
#include <thread>

class WakeWindow final : public mwtl::Window<WakeWindow> {
public:
    ~WakeWindow() noexcept { if (worker_.joinable()) worker_.join(); }

    void BuildUI() {
        SetTitle(L"Worker thread will wake this HWND safely");
        const mwtl::WindowWakeup wake = GetWakeup();
        worker_ = std::thread([wake] {
            ::Sleep(1000);
            wake.TryWake();
        });
    }

    BEGIN_MSG_MAP(WakeWindow)
        MESSAGE_HANDLER(mwtl::WindowWakeup::Message(), OnWake)
        CHAIN_MSG_MAP(mwtl::Window<WakeWindow>)
    END_MSG_MAP()

private:
    LRESULT OnWake(UINT, WPARAM, LPARAM, BOOL&) noexcept {
        SetTitle(L"Safe cross-thread wake-up received");
        return 0;
    }
    std::thread worker_;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<WakeWindow>(show_command); }
    catch (...) { return EXIT_FAILURE; }
}
