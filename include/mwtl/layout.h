#pragma once

#include <windows.h>

#include <concepts>
#include <limits>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include <mwtl/dpi.h>

namespace mwtl {

enum class LayoutDirection {
    row,
    column,
    overlay,
};

enum class CrossAlignment {
    stretch,
    start,
    center,
    end,
};

enum class LayoutLengthKind {
    auto_size,
    fixed,
    stretch,
};

struct LayoutLength {
    LayoutLengthKind kind = LayoutLengthKind::auto_size;
    Dip value{};
    Dip minimum{};
    Dip maximum{(std::numeric_limits<float>::infinity)()};
    float weight = 1.0f;

    [[nodiscard]] static constexpr LayoutLength Auto(
        Dip minimum = {},
        Dip maximum = Dip{(std::numeric_limits<float>::infinity)()}) noexcept {
        return {LayoutLengthKind::auto_size, {}, minimum, maximum, 0.0f};
    }

    [[nodiscard]] static constexpr LayoutLength Fixed(Dip value) noexcept {
        return {LayoutLengthKind::fixed, value, value, value, 0.0f};
    }

    [[nodiscard]] static constexpr LayoutLength Stretch(
        float weight = 1.0f,
        Dip minimum = {},
        Dip maximum = Dip{(std::numeric_limits<float>::infinity)()}) noexcept {
        return {LayoutLengthKind::stretch, {}, minimum, maximum, weight};
    }
};

[[nodiscard]] constexpr LayoutLength Auto(
    Dip minimum = {},
    Dip maximum = Dip{(std::numeric_limits<float>::infinity)()}) noexcept {
    return LayoutLength::Auto(minimum, maximum);
}

[[nodiscard]] constexpr LayoutLength Fixed(Dip value) noexcept {
    return LayoutLength::Fixed(value);
}

[[nodiscard]] constexpr LayoutLength Stretch(
    float weight = 1.0f,
    Dip minimum = {},
    Dip maximum = Dip{(std::numeric_limits<float>::infinity)()}) noexcept {
    return LayoutLength::Stretch(weight, minimum, maximum);
}

struct LayoutItemOptions {
    CrossAlignment alignment = CrossAlignment::stretch;
    Dip minimum_cross{};
    Dip maximum_cross{(std::numeric_limits<float>::infinity)()};
    std::optional<SizeDip> preferred_size;
    // Optional native HWND extent. A zero axis keeps the layout cell size.
    // This is useful for controls such as drop-down combo boxes whose window
    // height includes a normally hidden popup extent.
    std::optional<SizeDip> native_size;
};

class LayoutNode final {
public:
    explicit LayoutNode(LayoutDirection direction) noexcept;
    ~LayoutNode() noexcept;

    LayoutNode(const LayoutNode&) = delete;
    LayoutNode& operator=(const LayoutNode&) = delete;
    LayoutNode(LayoutNode&&) noexcept;
    LayoutNode& operator=(LayoutNode&&) noexcept;

    LayoutNode& Gap(Dip value) noexcept;
    LayoutNode& Margin(Thickness value) noexcept;
    LayoutNode& Margin(Dip uniform) noexcept;

    LayoutNode& Add(
        HWND window,
        LayoutLength length = LayoutLength::Auto(),
        LayoutItemOptions options = {});

    template <typename Control>
        requires requires(const Control& value) {
            { value.GetHwnd() } -> std::same_as<HWND>;
        }
    LayoutNode& Add(
        const Control& control,
        LayoutLength length = LayoutLength::Auto(),
        LayoutItemOptions options = {}) {
        return Add(control.GetHwnd(), length, std::move(options));
    }

    LayoutNode& Add(
        LayoutNode child,
        LayoutLength length = LayoutLength::Auto(),
        LayoutItemOptions options = {});

    [[nodiscard]] LayoutDirection GetDirection() const noexcept;
    [[nodiscard]] std::size_t GetCount() const noexcept;

private:
    struct Child;
    struct Placement;

    [[nodiscard]] SizeDip Measure(DpiContext dpi) const noexcept;
    void CollectPlacements(
        RectDip bounds,
        DpiContext dpi,
        std::vector<Placement>& output) const noexcept;
    [[nodiscard]] std::size_t CountWindows() const noexcept;

    LayoutDirection direction_ = LayoutDirection::column;
    Dip gap_{};
    Thickness margin_{};
    std::vector<Child> children_;

    friend class LayoutHost;
};

[[nodiscard]] LayoutNode Row() noexcept;
[[nodiscard]] LayoutNode Column() noexcept;
[[nodiscard]] LayoutNode Overlay() noexcept;

class LayoutHost final {
public:
    LayoutHost() noexcept = default;
    explicit LayoutHost(LayoutNode root) noexcept;

    LayoutHost(const LayoutHost&) = delete;
    LayoutHost& operator=(const LayoutHost&) = delete;
    LayoutHost(LayoutHost&&) = delete;
    LayoutHost& operator=(LayoutHost&&) = delete;

    void SetRoot(LayoutNode root) noexcept;
    [[nodiscard]] bool HasRoot() const noexcept;
    [[nodiscard]] SizeDip GetPreferredSize(HWND parent) const noexcept;
    [[nodiscard]] bool Arrange(HWND parent) const noexcept;

private:
    std::optional<LayoutNode> root_;
};

}  // namespace mwtl
