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
                wake.TryWake();
            }
        });
    }

    void OnWakeup() noexcept {
        SetTitle(L"Safe cross-thread wake-up received");
    }

private:
    std::jthread worker_;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<WakeWindow>(instance, show_command);
}
