#include <mwtl/mwtl.h>

#include <cstdlib>
#include <chrono>

namespace {
HWND demo_window = nullptr;
unsigned idle_ticks = 0;
}

class WaitWindow final : public mwtl::Window<WaitWindow> {
public:
    void BuildUI() {
        demo_window = GetHwnd();
        SetTitle(L"Wait-aware pump - idle ticks update this title");
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    using namespace std::chrono_literals;
    mwtl::WaitAwareMessagePump pump({
        .idle_interval = 500ms,
        .on_idle = [] {
        ++idle_ticks;
        wchar_t title[96]{};
        ::swprintf_s(title, L"Wait-aware pump - idle tick %u", idle_ticks);
        ::SetWindowTextW(demo_window, title);
        },
    });
    return mwtl::RunApplication<WaitWindow>(instance, show_command, pump);
}
