#pragma once

#include <windows.h>

#include <chrono>

#include <mwtl/concepts.h>
#include <mwtl/events.h>

namespace mwtl {

class UiTimer final {
public:
    UiTimer() noexcept = default;
    ~UiTimer() noexcept;

    UiTimer(const UiTimer&) = delete;
    UiTimer& operator=(const UiTimer&) = delete;
    UiTimer(UiTimer&& other) noexcept;
    UiTimer& operator=(UiTimer&& other) noexcept;

    [[nodiscard]] bool Start(
        HWND window,
        TimerId id,
        std::chrono::milliseconds interval) noexcept;
    template <WindowLike Window>
    [[nodiscard]] bool Start(
        const Window& window,
        TimerId id,
        std::chrono::milliseconds interval) noexcept {
        return Start(window.GetHwnd(), id, interval);
    }
    void Stop() noexcept;

    [[nodiscard]] bool IsRunning() const noexcept;
    [[nodiscard]] TimerId GetId() const noexcept { return id_; }

private:
    HWND window_ = nullptr;  // Non-owning; valid while the timer is running.
    TimerId id_{};
};

}  // namespace mwtl
