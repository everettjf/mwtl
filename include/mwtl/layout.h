#pragma once

#include <windows.h>

#include <limits>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include <concepts>

#include <mwtl/concepts.h>
#include <mwtl/dpi.h>

namespace mwtl {

template <typename T>
concept IntrinsicallyMeasurable = requires(const T& value, DpiContext dpi) {
    { value.GetPreferredSize(dpi) } -> std::same_as<SizeDip>;
};

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

    LayoutNode& Gap(Dip value) & noexcept;
    LayoutNode&& Gap(Dip value) && noexcept {
        Gap(value);
        return std::move(*this);
    }
    LayoutNode& Margin(Thickness value) & noexcept;
    LayoutNode&& Margin(Thickness value) && noexcept {
        Margin(value);
        return std::move(*this);
    }
    LayoutNode& Margin(Dip uniform) & noexcept;
    LayoutNode&& Margin(Dip uniform) && noexcept {
        Margin(uniform);
        return std::move(*this);
    }

    LayoutNode& Add(
        HWND window,
        LayoutLength length = LayoutLength::Auto(),
        LayoutItemOptions options = {}) &;
    LayoutNode&& Add(
        HWND window,
        LayoutLength length = LayoutLength::Auto(),
        LayoutItemOptions options = {}) && {
        Add(window, length, std::move(options));
        return std::move(*this);
    }

    template <WindowLike Control>
    LayoutNode& Add(
        const Control& control,
        LayoutLength length = LayoutLength::Auto(),
        LayoutItemOptions options = {}) & {
        if constexpr (IntrinsicallyMeasurable<Control>) {
            if (!options.preferred_size) {
                options.preferred_size = control.GetPreferredSize(
                    DpiContext::FromWindow(control.GetHwnd()));
            }
        }
        return Add(control.GetHwnd(), length, std::move(options));
    }
    template <WindowLike Control>
    LayoutNode&& Add(
        const Control& control,
        LayoutLength length = LayoutLength::Auto(),
        LayoutItemOptions options = {}) && {
        if constexpr (IntrinsicallyMeasurable<Control>) {
            if (!options.preferred_size) {
                options.preferred_size = control.GetPreferredSize(
                    DpiContext::FromWindow(control.GetHwnd()));
            }
        }
        Add(control.GetHwnd(), length, std::move(options));
        return std::move(*this);
    }

    template <WindowLike Control>
    LayoutNode& Add(const Control& control, Dip length,
                    LayoutItemOptions options = {}) & {
        return Add(control, Fixed(length), std::move(options));
    }
    template <WindowLike Control>
    LayoutNode&& Add(const Control& control, Dip length,
                     LayoutItemOptions options = {}) && {
        Add(control, Fixed(length), std::move(options));
        return std::move(*this);
    }

    LayoutNode& Add(
        LayoutNode child,
        LayoutLength length = LayoutLength::Auto(),
        LayoutItemOptions options = {}) &;
    LayoutNode&& Add(
        LayoutNode child,
        LayoutLength length = LayoutLength::Auto(),
        LayoutItemOptions options = {}) && {
        Add(std::move(child), length, std::move(options));
        return std::move(*this);
    }

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
