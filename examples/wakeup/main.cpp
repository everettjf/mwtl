#include <mwtl/mwtl.h>

#include <chrono>
#include <cstdlib>
#include <thread>

class WakeWindow final : public mwtl::Window<WakeWindow> {
public:
    void BuildUI() {
        SetTitle(L"Worker thread will wake this HWND safely");
        const mwtl::WindowWakeup wake = GetWakeup();
        worker_ = std::jthread([wake](std::stop_token stop) {
            using namespace std::chrono_literals;
            if (!stop.stop_requested()) {
                std::this_thread::sleep_for(1s);
            }
            if (!stop.stop_requested()) {
                static_cast<void>(wake.TryWake());
            }
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
    std::jthread worker_;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<WakeWindow>(show_command); }
    catch (...) { return EXIT_FAILURE; }
}
