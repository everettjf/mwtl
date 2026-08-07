#include <mwtl/mwtl.h>

#include <cstdlib>

namespace { HWND demo_window = nullptr; }

class WaitWindow final : public mwtl::Window<WaitWindow> {
public:
    void BuildUI() {
        demo_window = GetHwnd();
        SetTitle(L"Wait-aware pump - idle ticks update this title");
    }
};

class IdleDelegate final : public mwtl::WaitAwarePumpDelegate {
public:
    void OnIdle() override {
        ++ticks_;
        wchar_t title[96]{};
        ::swprintf_s(title, L"Wait-aware pump - idle tick %u", ticks_);
        ::SetWindowTextW(demo_window, title);
    }
    DWORD GetWaitTimeout() const noexcept override { return 500; }
private:
    unsigned ticks_ = 0;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    IdleDelegate delegate;
    mwtl::WaitAwareMessagePump pump({
        .idle_timeout_ms = 500,
        .delegate = &delegate,
    });
    return mwtl::RunApplication<WaitWindow>(instance, show_command, pump);
}
