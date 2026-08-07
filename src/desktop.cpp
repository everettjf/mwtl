#include <mwtl/desktop.h>

#include <algorithm>
#include <cstring>
#include <utility>

namespace mwtl {
namespace {

std::wstring Terminate(std::wstring_view value) {
    return std::wstring(value);
}

FileDialogResult ShowFileDialog(const FileDialogOptions& options, bool save) {
    constexpr DWORD kBufferCharacters = 32768;
    std::vector<wchar_t> path(kBufferCharacters, L'\0');
    const std::wstring initial_path = options.initial_path.native();
    if (!initial_path.empty()) {
        const auto count = (std::min)(initial_path.size(), path.size() - 1);
        std::copy_n(initial_path.data(), count, path.data());
    }

    std::wstring filters;
    for (const auto& filter : options.filters) {
        filters.append(filter.name).push_back(L'\0');
        filters.append(filter.pattern).push_back(L'\0');
    }
    if (!filters.empty()) filters.push_back(L'\0');

    OPENFILENAMEW dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = options.owner;
    dialog.lpstrFilter = filters.empty() ? nullptr : filters.c_str();
    dialog.lpstrFile = path.data();
    dialog.nMaxFile = static_cast<DWORD>(path.size());
    dialog.lpstrTitle = options.title.empty() ? nullptr : options.title.c_str();
    dialog.lpstrDefExt = options.default_extension.empty()
        ? nullptr : options.default_extension.c_str();
    dialog.Flags = options.flags | (save ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST) |
        (options.hook != nullptr ? OFN_ENABLEHOOK : 0);
    dialog.lpfnHook = options.hook;
    dialog.lCustData = options.custom_data;

    const BOOL accepted = save ? ::GetSaveFileNameW(&dialog) : ::GetOpenFileNameW(&dialog);
    if (accepted != FALSE) {
        return {std::filesystem::path(path.data()), 0, true};
    }
    return {{}, ::CommDlgExtendedError(), false};
}

RECT ClampRectToWorkArea(RECT rect) noexcept {
    const HMONITOR monitor = ::MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if (monitor == nullptr || ::GetMonitorInfoW(monitor, &info) == FALSE) {
        return rect;
    }
    const LONG width = (std::min)(rect.right - rect.left, info.rcWork.right - info.rcWork.left);
    const LONG height = (std::min)(rect.bottom - rect.top, info.rcWork.bottom - info.rcWork.top);
    rect.left = (std::clamp)(rect.left, info.rcWork.left, info.rcWork.right - width);
    rect.top = (std::clamp)(rect.top, info.rcWork.top, info.rcWork.bottom - height);
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;
    return rect;
}

}  // namespace

Menu::~Menu() noexcept { Reset(); }
Menu::Menu(Menu&& other) noexcept : menu_(std::exchange(other.menu_, nullptr)) {}
Menu& Menu::operator=(Menu&& other) noexcept {
    if (this != &other) { Reset(); menu_ = std::exchange(other.menu_, nullptr); }
    return *this;
}
bool Menu::Create() noexcept { Reset(); menu_ = ::CreateMenu(); return menu_ != nullptr; }
bool Menu::CreatePopup() noexcept { Reset(); menu_ = ::CreatePopupMenu(); return menu_ != nullptr; }
bool Menu::AppendCommand(UINT id, std::wstring_view text, bool enabled) {
    const std::wstring value = Terminate(text);
    return menu_ != nullptr && ::AppendMenuW(menu_, MF_STRING | (enabled ? MF_ENABLED : MF_GRAYED), id, value.c_str()) != FALSE;
}
bool Menu::AppendSeparator() noexcept {
    return menu_ != nullptr && ::AppendMenuW(menu_, MF_SEPARATOR, 0, nullptr) != FALSE;
}
bool Menu::AppendSubmenu(Menu&& submenu, std::wstring_view text, bool enabled) {
    if (menu_ == nullptr || submenu.menu_ == nullptr) return false;
    const std::wstring value = Terminate(text);
    if (::AppendMenuW(menu_, MF_POPUP | (enabled ? MF_ENABLED : MF_GRAYED),
                      reinterpret_cast<UINT_PTR>(submenu.menu_), value.c_str()) == FALSE) return false;
    submenu.menu_ = nullptr;
    return true;
}
bool Menu::AttachToWindow(HWND window) noexcept {
    if (menu_ == nullptr || window == nullptr || ::SetMenu(window, menu_) == FALSE) return false;
    menu_ = nullptr;
    return ::DrawMenuBar(window) != FALSE;
}
UINT Menu::Track(HWND owner, POINT point, UINT flags) const noexcept {
    return menu_ == nullptr ? 0 : ::TrackPopupMenu(menu_, flags, point.x, point.y, 0, owner, nullptr);
}
void Menu::Reset() noexcept { if (menu_ != nullptr) ::DestroyMenu(std::exchange(menu_, nullptr)); }

AcceleratorTable::~AcceleratorTable() noexcept { Reset(); }
AcceleratorTable::AcceleratorTable(AcceleratorTable&& other) noexcept : table_(std::exchange(other.table_, nullptr)) {}
AcceleratorTable& AcceleratorTable::operator=(AcceleratorTable&& other) noexcept {
    if (this != &other) { Reset(); table_ = std::exchange(other.table_, nullptr); }
    return *this;
}
bool AcceleratorTable::Create(std::span<const ACCEL> entries) noexcept {
    Reset();
    if (entries.empty() || entries.size() > static_cast<std::size_t>(INT_MAX)) return false;
    table_ = ::CreateAcceleratorTableW(const_cast<ACCEL*>(entries.data()), static_cast<int>(entries.size()));
    return table_ != nullptr;
}
void AcceleratorTable::Reset() noexcept { if (table_ != nullptr) ::DestroyAcceleratorTable(std::exchange(table_, nullptr)); }

FileDialogResult ShowOpenFileDialog(const FileDialogOptions& options) { return ShowFileDialog(options, false); }
FileDialogResult ShowSaveFileDialog(const FileDialogOptions& options) { return ShowFileDialog(options, true); }
FileDialogResult ShowFolderDialog(const FolderDialogOptions& options) {
    BROWSEINFOW browse{};
    browse.hwndOwner = options.owner;
    browse.lpszTitle = options.title.empty() ? nullptr : options.title.c_str();
    browse.ulFlags = options.flags;
    browse.lpfn = options.callback;
    browse.lParam = options.custom_data;
    PIDLIST_ABSOLUTE item = ::SHBrowseForFolderW(&browse);
    if (item == nullptr) return {{}, 0, false};
    struct FreeItem { PIDLIST_ABSOLUTE value; ~FreeItem() { ::CoTaskMemFree(value); } } free_item{item};
    std::wstring path(32768, L'\0');
    if (::SHGetPathFromIDListEx(item, path.data(), static_cast<DWORD>(path.size()), 0) == FALSE) {
        return {{}, ERROR_PATH_NOT_FOUND, false};
    }
    path.resize(std::char_traits<wchar_t>::length(path.c_str()));
    return {std::filesystem::path(std::move(path)), 0, true};
}

bool SetClipboardText(HWND owner, std::wstring_view text) noexcept {
    if (::OpenClipboard(owner) == FALSE) return false;
    struct Close { ~Close() { ::CloseClipboard(); } } close;
    if (::EmptyClipboard() == FALSE) return false;
    const SIZE_T bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL memory = ::GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (memory == nullptr) return false;
    void* target = ::GlobalLock(memory);
    if (target == nullptr) { ::GlobalFree(memory); return false; }
    std::memcpy(target, text.data(), text.size() * sizeof(wchar_t));
    static_cast<wchar_t*>(target)[text.size()] = L'\0';
    ::GlobalUnlock(memory);
    if (::SetClipboardData(CF_UNICODETEXT, memory) == nullptr) { ::GlobalFree(memory); return false; }
    return true;
}

std::wstring GetClipboardText(HWND owner) {
    if (::OpenClipboard(owner) == FALSE) return {};
    struct Close { ~Close() { ::CloseClipboard(); } } close;
    HANDLE memory = ::GetClipboardData(CF_UNICODETEXT);
    if (memory == nullptr) return {};
    const auto* text = static_cast<const wchar_t*>(::GlobalLock(memory));
    if (text == nullptr) return {};
    std::wstring result(text);
    ::GlobalUnlock(memory);
    return result;
}

void EnableFileDrop(HWND window, bool enabled) noexcept { if (window != nullptr) ::DragAcceptFiles(window, enabled ? TRUE : FALSE); }
std::vector<std::wstring> ReadDroppedFiles(HDROP drop) {
    std::vector<std::wstring> files;
    if (drop == nullptr) return files;
    struct Finish { HDROP value; ~Finish() { ::DragFinish(value); } } finish{drop};
    const UINT count = ::DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
    files.reserve(count);
    for (UINT index = 0; index < count; ++index) {
        const UINT length = ::DragQueryFileW(drop, index, nullptr, 0);
        std::wstring path(static_cast<std::size_t>(length) + 1, L'\0');
        const UINT copied = ::DragQueryFileW(drop, index, path.data(), length + 1);
        path.resize(copied);
        files.push_back(std::move(path));
    }
    return files;
}

bool CaptureWindowPlacement(HWND window, SavedWindowPlacement& output) noexcept {
    output.placement.length = sizeof(WINDOWPLACEMENT);
    return window != nullptr && ::GetWindowPlacement(window, &output.placement) != FALSE;
}
bool RestoreWindowPlacement(HWND window, const SavedWindowPlacement& saved, bool clamp) noexcept {
    if (window == nullptr || saved.placement.length != sizeof(WINDOWPLACEMENT)) return false;
    WINDOWPLACEMENT placement = saved.placement;
    if (placement.showCmd == SW_SHOWMINIMIZED || placement.showCmd == SW_MINIMIZE || placement.showCmd == SW_SHOWMINNOACTIVE) placement.showCmd = SW_SHOWNORMAL;
    if (clamp) placement.rcNormalPosition = ClampRectToWorkArea(placement.rcNormalPosition);
    return ::SetWindowPlacement(window, &placement) != FALSE;
}
bool SaveWindowPlacementToRegistry(HKEY root, std::wstring_view subkey, std::wstring_view value_name, const SavedWindowPlacement& placement) {
    const std::wstring key_name = Terminate(subkey), name = Terminate(value_name);
    HKEY key = nullptr;
    if (::RegCreateKeyExW(root, key_name.c_str(), 0, nullptr, 0, KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS) return false;
    const LONG status = ::RegSetValueExW(key, name.c_str(), 0, REG_BINARY,
        reinterpret_cast<const BYTE*>(&placement), sizeof(placement));
    ::RegCloseKey(key);
    return status == ERROR_SUCCESS;
}
bool LoadWindowPlacementFromRegistry(HKEY root, std::wstring_view subkey, std::wstring_view value_name, SavedWindowPlacement& placement) {
    const std::wstring key_name = Terminate(subkey), name = Terminate(value_name);
    HKEY key = nullptr;
    if (::RegOpenKeyExW(root, key_name.c_str(), 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) return false;
    DWORD type = 0, size = sizeof(placement);
    SavedWindowPlacement loaded{};
    const LONG status = ::RegQueryValueExW(key, name.c_str(), nullptr, &type,
        reinterpret_cast<BYTE*>(&loaded), &size);
    ::RegCloseKey(key);
    if (status != ERROR_SUCCESS || type != REG_BINARY || size != sizeof(loaded) || loaded.placement.length != sizeof(WINDOWPLACEMENT)) return false;
    placement = loaded;
    return true;
}

UiFont::~UiFont() noexcept { Reset(); }
UiFont::UiFont(UiFont&& other) noexcept : font_(std::exchange(other.font_, nullptr)) {}
UiFont& UiFont::operator=(UiFont&& other) noexcept { if (this != &other) { Reset(); font_ = std::exchange(other.font_, nullptr); } return *this; }
bool UiFont::CreateMessageFont(UINT dpi) noexcept {
    Reset();
    NONCLIENTMETRICSW metrics{sizeof(metrics)};
    if (::SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0, dpi == 0 ? 96 : dpi) == FALSE) return false;
    font_ = ::CreateFontIndirectW(&metrics.lfMessageFont);
    return font_ != nullptr;
}
void UiFont::Reset() noexcept { if (font_ != nullptr) ::DeleteObject(std::exchange(font_, nullptr)); }
void SetControlFont(HWND control, HFONT font, bool redraw) noexcept { if (control != nullptr) ::SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(font), redraw ? TRUE : FALSE); }
HWND FocusNextControl(HWND parent, HWND current, bool previous) noexcept {
    if (parent == nullptr) return nullptr;
    HWND next = ::GetNextDlgTabItem(parent, current, previous ? TRUE : FALSE);
    return next != nullptr ? ::SetFocus(next) : nullptr;
}

}  // namespace mwtl
