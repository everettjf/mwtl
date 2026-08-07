#pragma once

#include <windows.h>

#include <compare>

namespace mwtl {

struct Dip {
    float value = 0.0f;

    constexpr Dip() noexcept = default;
    constexpr explicit Dip(float input) noexcept : value(input) {}

    constexpr auto operator<=>(const Dip&) const noexcept = default;
};

constexpr Dip operator+(Dip left, Dip right) noexcept {
    return Dip(left.value + right.value);
}

constexpr Dip operator-(Dip left, Dip right) noexcept {
    return Dip(left.value - right.value);
}

constexpr Dip operator-(Dip value) noexcept {
    return Dip(-value.value);
}

struct PointDip {
    Dip x{};
    Dip y{};
};

struct SizeDip {
    Dip width{};
    Dip height{};
};

struct RectDip {
    PointDip origin{};
    SizeDip size{};
};

struct Thickness {
    Dip left{};
    Dip top{};
    Dip right{};
    Dip bottom{};
};

constexpr Dip operator""_dip(long double value) noexcept {
    return Dip(static_cast<float>(value));
}

constexpr Dip operator""_dip(unsigned long long value) noexcept {
    return Dip(static_cast<float>(value));
}

class DpiContext final {
public:
    static constexpr UINT kDefaultDpi = 96;

    constexpr DpiContext() noexcept = default;
    explicit constexpr DpiContext(UINT dpi) noexcept
        : dpi_(dpi == 0 ? kDefaultDpi : dpi) {}

    static DpiContext FromWindow(HWND window) noexcept;
    static constexpr DpiContext FromDpi(UINT dpi) noexcept {
        return DpiContext(dpi);
    }

    [[nodiscard]] constexpr UINT GetDpi() const noexcept { return dpi_; }
    [[nodiscard]] constexpr float GetScale() const noexcept {
        return static_cast<float>(dpi_) / static_cast<float>(kDefaultDpi);
    }

    [[nodiscard]] int ToPixels(Dip value) const noexcept;
    [[nodiscard]] Dip FromPixels(int value) const noexcept;
    [[nodiscard]] POINT ToPixels(PointDip value) const noexcept;
    [[nodiscard]] SIZE ToPixels(SizeDip value) const noexcept;
    [[nodiscard]] RECT ToPixels(RectDip value) const noexcept;

private:
    UINT dpi_ = kDefaultDpi;
};

}  // namespace mwtl
