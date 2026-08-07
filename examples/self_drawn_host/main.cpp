#include <mwtl/mwtl.h>

#include <atomic>
#include <cstdlib>
#include <thread>

class SelfDrawnWindow final : public mwtl::Window<SelfDrawnWindow> {
public:
    ~SelfDrawnWindow() noexcept {
        stopping_.store(true, std::memory_order_release);
        if (producer_.joinable()) producer_.join();
    }

    void BuildUI() {
        SetTitle(L"Self-drawn host - worker-driven dirty frames");
        const mwtl::WindowWakeup wake = GetWakeup();
        producer_ = std::thread([this, wake] {
            while (!stopping_.load(std::memory_order_acquire)) {
                ::Sleep(16);
                if (!wake.TryWake()) break;
            }
        });
    }

    BEGIN_MSG_MAP(SelfDrawnWindow)
        MESSAGE_HANDLER(mwtl::WindowWakeup::Message(), OnRenderDirty)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        CHAIN_MSG_MAP(mwtl::Window<SelfDrawnWindow>)
    END_MSG_MAP()

private:
    LRESULT OnRenderDirty(UINT, WPARAM, LPARAM, BOOL&) noexcept {
        ++frame_;
        ::InvalidateRect(GetHwnd(), nullptr, FALSE);
        return 0;
    }

    LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL&) noexcept {
        PAINTSTRUCT paint{};
        const HDC dc = ::BeginPaint(GetHwnd(), &paint);
        if (dc != nullptr) {
            RECT client{};
            ::GetClientRect(GetHwnd(), &client);
            ::FillRect(dc, &client, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
            const int width = client.right > 160 ? client.right - 160 : 1;
            const int x = 40 + static_cast<int>((frame_ * 4) % static_cast<unsigned>(width));
            RECT marker{x, 80, x + 120, 160};
            ::SetDCBrushColor(dc, RGB(33, 115, 190));
            ::FillRect(dc, &marker, static_cast<HBRUSH>(::GetStockObject(DC_BRUSH)));
            ::EndPaint(GetHwnd(), &paint);
        }
        return 0;
    }

    std::atomic<bool> stopping_{false};
    std::thread producer_;
    unsigned frame_ = 0;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try {
        mwtl::WaitAwareMessagePump pump({nullptr, 0, INFINITE, nullptr});
        return mwtl::Application(instance).Run<SelfDrawnWindow>(
            show_command, mwtl::WindowOptions{}, pump);
    } catch (...) { return EXIT_FAILURE; }
}
