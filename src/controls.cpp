#include <mwtl/controls.h>

#include <utility>

namespace mwtl {
namespace {

[[nodiscard]] RECT ResolveControlBounds(HWND parent, RectDip bounds) noexcept {
    return DpiContext::FromWindow(parent).ToPixels(bounds);
}

void ApplyDefaultFont(HWND window) noexcept {
    const HFONT font = static_cast<HFONT>(::GetStockObject(DEFAULT_GUI_FONT));
    ::SendMessageW(window, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
}

}  // namespace

NativeControl::~NativeControl() noexcept {
    Destroy();
}

NativeControl::NativeControl(NativeControl&& other) noexcept
    : window_(std::exchange(other.window_, nullptr)),
      parent_(std::exchange(other.parent_, nullptr)),
      id_(std::exchange(other.id_, {})) {}

NativeControl& NativeControl::operator=(NativeControl&& other) noexcept {
    if (this != &other) {
        Destroy();
        window_ = std::exchange(other.window_, nullptr);
        parent_ = std::exchange(other.parent_, nullptr);
        id_ = std::exchange(other.id_, {});
    }
    return *this;
}

bool NativeControl::IsWindow() const noexcept {
    return window_ != nullptr && parent_ != nullptr && id_.value > 0 &&
        ::IsWindow(window_) != FALSE && ::IsWindow(parent_) != FALSE &&
        ::GetParent(window_) == parent_ && ::GetDlgCtrlID(window_) == id_.value;
}

bool NativeControl::SetText(std::wstring_view text) {
    if (!IsWindow()) {
        ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return false;
    }
    const std::wstring terminated{text};
    return ::SetWindowTextW(window_, terminated.c_str()) != FALSE;
}

std::wstring NativeControl::GetText() const {
    if (!IsWindow()) {
        return {};
    }
    const int length = ::GetWindowTextLengthW(window_);
    if (length <= 0) {
        return {};
    }
    std::wstring text(static_cast<std::size_t>(length) + 1, L'\0');
    const int copied = ::GetWindowTextW(
        window_, text.data(), static_cast<int>(text.size()));
    text.resize(copied > 0 ? static_cast<std::size_t>(copied) : 0);
    return text;
}

bool NativeControl::SetBounds(RectDip bounds) noexcept {
    if (!IsWindow()) {
        ::SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return false;
    }
    const HWND parent = ::GetParent(window_);
    const RECT pixels = ResolveControlBounds(parent, bounds);
    return ::SetWindowPos(
               window_, nullptr, pixels.left, pixels.top,
               pixels.right - pixels.left, pixels.bottom - pixels.top,
               SWP_NOZORDER | SWP_NOACTIVATE) != FALSE;
}

void NativeControl::SetEnabled(bool enabled) noexcept {
    if (IsWindow()) {
        ::EnableWindow(window_, enabled ? TRUE : FALSE);
    }
}

void NativeControl::SetVisible(bool visible) noexcept {
    if (IsWindow()) {
        ::ShowWindow(window_, visible ? SW_SHOW : SW_HIDE);
    }
}

void NativeControl::Focus() noexcept {
    if (IsWindow()) {
        ::SetFocus(window_);
    }
}

void NativeControl::Destroy() noexcept {
    if (IsWindow()) {
        ::DestroyWindow(window_);
    }
    window_ = nullptr;
    parent_ = nullptr;
    id_ = {};
}

bool NativeControl::CreateNative(
    const wchar_t* class_name,
    HWND parent,
    ControlId id,
    std::wstring_view text,
    RectDip bounds,
    DWORD style,
    DWORD extended_style) {
    Destroy();
    if (parent == nullptr || id.value <= 0) {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }

    const RECT pixels = ResolveControlBounds(parent, bounds);
    const std::wstring terminated{text};
    window_ = ::CreateWindowExW(
        extended_style,
        class_name,
        terminated.c_str(),
        style,
        pixels.left,
        pixels.top,
        pixels.right - pixels.left,
        pixels.bottom - pixels.top,
        parent,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id.value)),
        reinterpret_cast<HINSTANCE>(::GetWindowLongPtrW(parent, GWLP_HINSTANCE)),
        nullptr);
    if (window_ == nullptr) {
        return false;
    }
    parent_ = parent;
    id_ = id;
    ApplyDefaultFont(window_);
    return true;
}

bool Label::Create(
    HWND parent,
    ControlId id,
    std::wstring_view text,
    RectDip bounds,
    LabelOptions options) {
    return CreateNative(
        L"STATIC", parent, id, text, bounds, options.style,
        options.extended_style);
}

bool Button::Create(
    HWND parent,
    ControlId id,
    std::wstring_view text,
    RectDip bounds,
    ButtonOptions options) {
    return CreateNative(
        L"BUTTON", parent, id, text, bounds, options.style,
        options.extended_style);
}

void Button::Click() noexcept {
    if (IsWindow()) {
        ::SendMessageW(GetHwnd(), BM_CLICK, 0, 0);
    }
}

bool TextBox::Create(
    HWND parent,
    ControlId id,
    std::wstring_view text,
    RectDip bounds,
    TextBoxOptions options) {
    return CreateNative(
        L"EDIT", parent, id, text, bounds, options.style,
        options.extended_style);
}

void TextBox::SelectAll() noexcept {
    if (IsWindow()) {
        ::SendMessageW(GetHwnd(), EM_SETSEL, 0, -1);
    }
}

void TextBox::SetReadOnly(bool read_only) noexcept {
    if (IsWindow()) {
        ::SendMessageW(GetHwnd(), EM_SETREADONLY, read_only ? TRUE : FALSE, 0);
    }
}

bool CheckBox::Create(
    HWND parent,
    ControlId id,
    std::wstring_view text,
    RectDip bounds,
    CheckBoxOptions options) {
    return CreateNative(
        L"BUTTON", parent, id, text, bounds, options.style,
        options.extended_style);
}

bool CheckBox::IsChecked() const noexcept {
    return IsWindow() &&
        ::SendMessageW(GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void CheckBox::SetChecked(bool checked) noexcept {
    if (IsWindow()) {
        ::SendMessageW(
            GetHwnd(), BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
    }
}

bool RadioButton::Create(HWND parent, ControlId id, std::wstring_view text, RectDip bounds, RadioButtonOptions options) {
    return CreateNative(L"BUTTON", parent, id, text, bounds, options.style, options.extended_style);
}

bool RadioButton::IsChecked() const noexcept {
    return IsWindow() && ::SendMessageW(GetHwnd(), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void RadioButton::SetChecked(bool checked) noexcept {
    if (IsWindow()) {
        ::SendMessageW(GetHwnd(), BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
    }
}

bool GroupBox::Create(HWND parent, ControlId id, std::wstring_view text, RectDip bounds, GroupBoxOptions options) {
    return CreateNative(L"BUTTON", parent, id, text, bounds, options.style, options.extended_style);
}

bool ListBox::Create(HWND parent, ControlId id, RectDip bounds, ListBoxOptions options) {
    return CreateNative(L"LISTBOX", parent, id, L"", bounds, options.style, options.extended_style);
}

int ListBox::AddItem(std::wstring_view text) {
    if (!IsWindow()) {
        return LB_ERR;
    }
    const std::wstring terminated{text};
    return static_cast<int>(::SendMessageW(GetHwnd(), LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(terminated.c_str())));
}

int ListBox::GetSelection() const noexcept {
    return IsWindow() ? static_cast<int>(::SendMessageW(GetHwnd(), LB_GETCURSEL, 0, 0)) : LB_ERR;
}

bool ListBox::SetSelection(int index) noexcept {
    return IsWindow() && ::SendMessageW(GetHwnd(), LB_SETCURSEL, index, 0) != LB_ERR;
}

bool ComboBox::Create(
    HWND parent,
    ControlId id,
    RectDip bounds,
    ComboBoxOptions options) {
    return CreateNative(
        L"COMBOBOX", parent, id, L"", bounds, options.style,
        options.extended_style);
}

int ComboBox::AddItem(std::wstring_view text) {
    if (!IsWindow()) {
        return CB_ERR;
    }
    const std::wstring terminated{text};
    return static_cast<int>(::SendMessageW(
        GetHwnd(), CB_ADDSTRING, 0,
        reinterpret_cast<LPARAM>(terminated.c_str())));
}

int ComboBox::GetSelection() const noexcept {
    return IsWindow()
        ? static_cast<int>(::SendMessageW(GetHwnd(), CB_GETCURSEL, 0, 0))
        : CB_ERR;
}

bool ComboBox::SetSelection(int index) noexcept {
    return IsWindow() &&
        ::SendMessageW(GetHwnd(), CB_SETCURSEL, index, 0) != CB_ERR;
}

bool ProgressBar::Create(
    HWND parent,
    ControlId id,
    RectDip bounds,
    ProgressBarOptions options) {
    INITCOMMONCONTROLSEX controls{
        sizeof(controls), ICC_PROGRESS_CLASS};
    if (::InitCommonControlsEx(&controls) == FALSE) {
        return false;
    }
    return CreateNative(
        PROGRESS_CLASSW, parent, id, L"", bounds, options.style,
        options.extended_style);
}

void ProgressBar::SetRange(int minimum, int maximum) noexcept {
    if (IsWindow()) {
        ::SendMessageW(GetHwnd(), PBM_SETRANGE32, minimum, maximum);
    }
}

void ProgressBar::SetValue(int value) noexcept {
    if (IsWindow()) {
        ::SendMessageW(GetHwnd(), PBM_SETPOS, value, 0);
    }
}

int ProgressBar::GetValue() const noexcept {
    return IsWindow()
        ? static_cast<int>(::SendMessageW(GetHwnd(), PBM_GETPOS, 0, 0))
        : 0;
}

bool Slider::Create(HWND parent, ControlId id, RectDip bounds, SliderOptions options) {
    INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_BAR_CLASSES};
    if (::InitCommonControlsEx(&controls) == FALSE) {
        return false;
    }
    return CreateNative(TRACKBAR_CLASSW, parent, id, L"", bounds, options.style, options.extended_style);
}

void Slider::SetRange(int minimum, int maximum) noexcept {
    if (IsWindow()) {
        ::SendMessageW(GetHwnd(), TBM_SETRANGEMIN, FALSE, minimum);
        ::SendMessageW(GetHwnd(), TBM_SETRANGEMAX, TRUE, maximum);
    }
}

void Slider::SetValue(int value) noexcept {
    if (IsWindow()) {
        ::SendMessageW(GetHwnd(), TBM_SETPOS, TRUE, value);
    }
}

int Slider::GetValue() const noexcept {
    return IsWindow() ? static_cast<int>(::SendMessageW(GetHwnd(), TBM_GETPOS, 0, 0)) : 0;
}

}  // namespace mwtl
