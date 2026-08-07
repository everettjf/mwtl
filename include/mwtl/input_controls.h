#pragma once

#include <windows.h>
#include <commctrl.h>

#include <concepts>
#include <optional>
#include <string_view>

#include <mwtl/concepts.h>
#include <mwtl/controls.h>

namespace mwtl {

struct DateTimePickerOptions { DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | DTS_SHORTDATECENTURYFORMAT; DWORD extended_style = 0; };
class DateTimePicker final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, DateTimePickerOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, DateTimePickerOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] std::optional<SYSTEMTIME> GetValue() const noexcept;
    [[nodiscard]] bool SetValue(const SYSTEMTIME& value) noexcept;
};

struct MonthCalendarOptions { DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP; DWORD extended_style = 0; };
class MonthCalendar final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, MonthCalendarOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, MonthCalendarOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] std::optional<SYSTEMTIME> GetValue() const noexcept;
    [[nodiscard]] bool SetValue(const SYSTEMTIME& value) noexcept;
};

struct HotKeyOptions { DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP; DWORD extended_style = WS_EX_CLIENTEDGE; };
class HotKey final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, HotKeyOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, HotKeyOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    HotKey& SetValue(WORD virtual_key, WORD modifiers) noexcept;
    [[nodiscard]] DWORD GetValue() const noexcept;
};

struct IpAddressOptions { DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP; DWORD extended_style = WS_EX_CLIENTEDGE; };
class IpAddress final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, IpAddressOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, IpAddressOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    IpAddress& SetValue(BYTE first, BYTE second, BYTE third, BYTE fourth) noexcept;
    [[nodiscard]] std::optional<DWORD> GetValue() const noexcept;
};

struct UpDownOptions { DWORD style = WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT; DWORD extended_style = 0; };
class UpDown final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, UpDownOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, UpDownOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    UpDown& SetBuddy(const NativeControl& buddy) noexcept;
    UpDown& SetRange(int minimum, int maximum) noexcept;
    UpDown& SetValue(int value) noexcept;
    [[nodiscard]] int GetValue() const noexcept;
};

struct SysLinkOptions { DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP; DWORD extended_style = 0; };
class SysLink final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, std::wstring_view markup, RectDip bounds, SysLinkOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, std::wstring_view markup, RectDip bounds, SysLinkOptions options = {}) { return Create(parent.GetHwnd(), id, markup, bounds, options); }
};

}  // namespace mwtl
