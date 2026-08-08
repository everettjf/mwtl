#include <mwtl/mwtl.h>

#include <cstdlib>
#include <chrono>
#include <thread>

class SelfDrawnWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        SetTitle(L"Self-drawn host - worker-driven dirty frames");
        const mwtl::WindowWakeup wake = GetWakeup();
        producer_ = std::jthread([wake](std::stop_token stop) {
            while (!stop.stop_requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
                if (!wake.TryWake()) break;
            }
        });
    }

    mwtl::EventResult OnWakeup() noexcept override {
        ++frame_;
        ::InvalidateRect(GetHwnd(), nullptr, FALSE);
        return mwtl::EventResult::Handled();
    }

    mwtl::EventResult OnPaint(mwtl::PaintEvent& event) noexcept override {
        const HDC dc = event.GetDC();
        if (dc != nullptr) {
            RECT client{};
            ::GetClientRect(GetHwnd(), &client);
            ::FillRect(dc, &client, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
            const int width = client.right > 160 ? client.right - 160 : 1;
            const int x = 40 + static_cast<int>((frame_ * 4) % static_cast<unsigned>(width));
            RECT marker{x, 80, x + 120, 160};
            ::SetDCBrushColor(dc, RGB(33, 115, 190));
            ::FillRect(dc, &marker, static_cast<HBRUSH>(::GetStockObject(DC_BRUSH)));
        }
        return mwtl::EventResult::Handled();
    }

private:
    std::jthread producer_;
    unsigned frame_ = 0;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    mwtl::WaitAwareMessagePump pump;
    return mwtl::RunApplication<SelfDrawnWindow>(instance, show_command, pump);
}
