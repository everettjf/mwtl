#pragma once

#include <windows.h>
#include <commctrl.h>

#include <string>
#include <string_view>

#include <mwtl/dpi.h>
#include <mwtl/events.h>

namespace mwtl {

class NativeControl {
public:
    NativeControl() noexcept = default;
    ~NativeControl() noexcept;

    NativeControl(const NativeControl&) = delete;
    NativeControl& operator=(const NativeControl&) = delete;
    NativeControl(NativeControl&& other) noexcept;
    NativeControl& operator=(NativeControl&& other) noexcept;

    [[nodiscard]] HWND GetHwnd() const noexcept {
        return IsWindow() ? window_ : nullptr;
    }
    [[nodiscard]] bool IsWindow() const noexcept;
    [[nodiscard]] ControlId GetId() const noexcept { return id_; }

    [[nodiscard]] bool SetText(std::wstring_view text);
    [[nodiscard]] std::wstring GetText() const;
    [[nodiscard]] bool SetBounds(RectDip bounds) noexcept;
    void SetEnabled(bool enabled) noexcept;
    void SetVisible(bool visible) noexcept;
    void Focus() noexcept;
    void Destroy() noexcept;

protected:
    [[nodiscard]] bool CreateNative(
        const wchar_t* class_name,
        HWND parent,
        ControlId id,
        std::wstring_view text,
        RectDip bounds,
        DWORD style,
        DWORD extended_style = 0);

private:
    HWND window_ = nullptr;  // Owned child HWND while valid.
    HWND parent_ = nullptr;  // Non-owning identity check.
    ControlId id_{};
};

struct LabelOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | SS_LEFT;
    DWORD extended_style = 0;
};

class Label final : public NativeControl {
public:
    [[nodiscard]] bool Create(
        HWND parent,
        ControlId id,
        std::wstring_view text,
        RectDip bounds,
        LabelOptions options = {});
};

struct ButtonOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON;
    DWORD extended_style = 0;
};

class Button final : public NativeControl {
public:
    [[nodiscard]] bool Create(
        HWND parent,
        ControlId id,
        std::wstring_view text,
        RectDip bounds,
        ButtonOptions options = {});
    void Click() noexcept;
};

struct TextBoxOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL;
    DWORD extended_style = WS_EX_CLIENTEDGE;
};

class TextBox final : public NativeControl {
public:
    [[nodiscard]] bool Create(
        HWND parent,
        ControlId id,
        std::wstring_view text,
        RectDip bounds,
        TextBoxOptions options = {});
    void SelectAll() noexcept;
    void SetReadOnly(bool read_only) noexcept;
};

struct CheckBoxOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX;
    DWORD extended_style = 0;
};

class CheckBox final : public NativeControl {
public:
    [[nodiscard]] bool Create(
        HWND parent,
        ControlId id,
        std::wstring_view text,
        RectDip bounds,
        CheckBoxOptions options = {});
    [[nodiscard]] bool IsChecked() const noexcept;
    void SetChecked(bool checked) noexcept;
};

struct ComboBoxOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
        CBS_DROPDOWNLIST;
    DWORD extended_style = 0;
};

class ComboBox final : public NativeControl {
public:
    [[nodiscard]] bool Create(
        HWND parent,
        ControlId id,
        RectDip bounds,
        ComboBoxOptions options = {});
    [[nodiscard]] int AddItem(std::wstring_view text);
    [[nodiscard]] int GetSelection() const noexcept;
    [[nodiscard]] bool SetSelection(int index) noexcept;
};

struct ProgressBarOptions {
    DWORD style = WS_CHILD | WS_VISIBLE | PBS_SMOOTH;
    DWORD extended_style = 0;
};

class ProgressBar final : public NativeControl {
public:
    [[nodiscard]] bool Create(
        HWND parent,
        ControlId id,
        RectDip bounds,
        ProgressBarOptions options = {});
    void SetRange(int minimum, int maximum) noexcept;
    void SetValue(int value) noexcept;
    [[nodiscard]] int GetValue() const noexcept;
};

}  // namespace mwtl
