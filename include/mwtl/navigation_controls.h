#pragma once

#include <windows.h>
#include <commctrl.h>

#include <concepts>
#include <string_view>

#include <mwtl/concepts.h>
#include <mwtl/controls.h>

namespace mwtl {

struct TreeViewOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER |
        TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
    DWORD extended_style = WS_EX_CLIENTEDGE;
};

class TreeView final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, TreeViewOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, TreeViewOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] HTREEITEM AddItem(std::wstring_view text, HTREEITEM parent = TVI_ROOT, HTREEITEM after = TVI_LAST);
    [[nodiscard]] bool Expand(HTREEITEM item, bool expanded = true) noexcept;
};

struct ListViewOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
    DWORD extended_style = WS_EX_CLIENTEDGE;
};

class ListView final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, ListViewOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, ListViewOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] int AddColumn(std::wstring_view text, int width_pixels, int index = -1);
    [[nodiscard]] int AddItem(std::wstring_view text, int index = -1);
    [[nodiscard]] bool SetSubItem(int item, int sub_item, std::wstring_view text);
    ListView& SetExtendedListStyle(DWORD style) noexcept;
};

struct HeaderOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ;
    DWORD extended_style = 0;
};

class Header final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, HeaderOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, HeaderOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] int AddColumn(std::wstring_view text, int width_pixels, int index = -1);
};

struct TabControlOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TCS_TABS;
    DWORD extended_style = 0;
};

class TabControl final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, TabControlOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, TabControlOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] int AddTab(std::wstring_view text, int index = -1);
    [[nodiscard]] int GetSelection() const noexcept;
    [[nodiscard]] bool SetSelection(int index) noexcept;
};

struct ComboBoxExOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST;
    DWORD extended_style = 0;
};

class ComboBoxEx final : public NativeControl {
public:
    [[nodiscard]] bool Create(HWND parent, ControlId id, RectDip bounds, ComboBoxExOptions options = {});
    template <WindowLike Parent>
    [[nodiscard]] bool Create(const Parent& parent, ControlId id, RectDip bounds, ComboBoxExOptions options = {}) { return Create(parent.GetHwnd(), id, bounds, options); }
    [[nodiscard]] int AddItem(std::wstring_view text, int index = -1);
    [[nodiscard]] int GetSelection() const noexcept;
    [[nodiscard]] bool SetSelection(int index) noexcept;
};

}  // namespace mwtl
