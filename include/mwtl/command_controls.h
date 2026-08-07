#pragma once

#include <windows.h>
#include <commctrl.h>

#include <concepts>
#include <span>
#include <string_view>

#include <mwtl/concepts.h>
#include <mwtl/controls.h>

namespace mwtl {

struct ToolbarOptions { DWORD style = WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN; DWORD extended_style = 0; };
class Toolbar final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, ToolbarOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, ToolbarOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] bool AddTextButton(ControlId command, std::wstring_view text);
    void AutoSize() noexcept;
};

struct StatusBarOptions { DWORD style = WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP; DWORD extended_style = 0; };
class StatusBar final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, StatusBarOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, StatusBarOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] bool SetParts(std::span<const int> right_edges) noexcept;
    [[nodiscard]] bool SetPartText(int part, std::wstring_view text);
};

struct RebarOptions { DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | RBS_VARHEIGHT | CCS_NODIVIDER; DWORD extended_style = WS_EX_TOOLWINDOW; };
class Rebar final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, RebarOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, RebarOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] bool AddBand(const NativeControl& child, std::wstring_view text, int minimum_width);
};

struct PagerOptions { DWORD style = WS_CHILD | WS_VISIBLE | PGS_HORZ; DWORD extended_style = 0; };
class Pager final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, PagerOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, PagerOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    void SetChild(const NativeControl& child) noexcept;
    void SetButtonSize(int pixels) noexcept;
};

struct AnimationOptions { DWORD style = WS_CHILD | WS_VISIBLE | ACS_CENTER | ACS_TRANSPARENT; DWORD extended_style = 0; };
class Animation final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, AnimationOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, AnimationOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] bool Open(HINSTANCE module, UINT resource_id) noexcept;
    [[nodiscard]] bool Play(UINT repeat_count = UINT_MAX, UINT from = 0, UINT to = UINT_MAX) noexcept;
    void Stop() noexcept;
};

}  // namespace mwtl
