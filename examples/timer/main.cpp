#include <mwtl/mwtl.h>

#include <chrono>
#include <cstdlib>
#include <exception>
#include <stdexcept>

using namespace std::chrono_literals;

class TimerWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        ::SetWindowPos(GetHwnd(), nullptr, 0, 0, 900, 560,
                       SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        if (!SetTitle(L"Timer demo — waiting for WM_TIMER")) {
            throw std::runtime_error("SetTitle failed");
        }
        if (!timer_.Start(*this, kTimer, 1s)) {
            throw std::runtime_error("UiTimer::Start failed");
        }
    }

    mwtl::EventResult OnTimer(mwtl::TimerId timer_id) override {
        if (timer_id != kTimer) {
            return mwtl::EventResult::Propagate();
        }
        wchar_t title[96]{};
        _snwprintf_s(title, _countof(title), _TRUNCATE,
                     L"WM_TIMER tick %u", ++ticks_);
        SetTitle(title);
        return mwtl::EventResult::Handled();
    }

private:
    static constexpr mwtl::TimerId kTimer{1};
    mwtl::UiTimer timer_;
    unsigned int ticks_ = 0;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<TimerWindow>(instance, show_command);
}
