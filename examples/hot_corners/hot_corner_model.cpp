#include "hot_corner_model.h"

#include <algorithm>

namespace hot_corners {

std::optional<Hit> Detect(POINT cursor, std::span<const RECT> monitors,
                          LONG tolerance) noexcept {
    tolerance = (std::max<LONG>)(1, tolerance);
    for (std::size_t index = 0; index < monitors.size(); ++index) {
        const RECT& area = monitors[index];
        if (cursor.x < area.left || cursor.x >= area.right ||
            cursor.y < area.top || cursor.y >= area.bottom) {
            continue;
        }
        const bool left = cursor.x < area.left + tolerance;
        const bool right = cursor.x >= area.right - tolerance;
        const bool top = cursor.y < area.top + tolerance;
        const bool bottom = cursor.y >= area.bottom - tolerance;
        if (left && top) return Hit{index, Corner::top_left};
        if (right && top) return Hit{index, Corner::top_right};
        if (left && bottom) return Hit{index, Corner::bottom_left};
        if (right && bottom) return Hit{index, Corner::bottom_right};
    }
    return std::nullopt;
}

Action ResolveAction(const Settings& settings, Hit hit) noexcept {
    const auto corner = static_cast<std::size_t>(hit.corner);
    if (hit.monitor >= settings.monitors.size() || corner >= 4) return Action::none;
    return settings.monitors[hit.monitor].corners[corner];
}

std::optional<Hit> DwellTracker::Update(std::optional<Hit> hit,
                                        std::uint64_t now_ms,
                                        std::uint64_t dwell_ms) noexcept {
    if (hit != current_) {
        current_ = hit;
        entered_at_ = now_ms;
        fired_ = false;
    }
    if (!current_.has_value() || fired_ || now_ms - entered_at_ < dwell_ms) {
        return std::nullopt;
    }
    fired_ = true;
    return current_;
}

void DwellTracker::Reset() noexcept {
    current_.reset();
    entered_at_ = 0;
    fired_ = false;
}

}  // namespace hot_corners
