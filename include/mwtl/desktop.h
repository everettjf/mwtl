#pragma once

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace mwtl {

class Menu final {
public:
    Menu() noexcept = default;
    ~Menu() noexcept;
    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;
    Menu(Menu&& other) noexcept;
    Menu& operator=(Menu&& other) noexcept;

    [[nodiscard]] bool Create() noexcept;
    [[nodiscard]] bool CreatePopup() noexcept;
    [[nodiscard]] bool AppendCommand(UINT id, std::wstring_view text,
                                     bool enabled = true);
    [[nodiscard]] bool AppendSeparator() noexcept;
    [[nodiscard]] bool AppendSubmenu(Menu&& submenu, std::wstring_view text,
                                     bool enabled = true);
    [[nodiscard]] bool AttachToWindow(HWND window) noexcept;
    [[nodiscard]] UINT Track(HWND owner, POINT screen_position,
                             UINT flags = TPM_RIGHTBUTTON | TPM_RETURNCMD) const noexcept;
    [[nodiscard]] HMENU GetHandle() const noexcept { return menu_; }
    void Reset() noexcept;

private:
    HMENU menu_ = nullptr;
};

class AcceleratorTable final {
public:
    AcceleratorTable() noexcept = default;
    ~AcceleratorTable() noexcept;
    AcceleratorTable(const AcceleratorTable&) = delete;
    AcceleratorTable& operator=(const AcceleratorTable&) = delete;
    AcceleratorTable(AcceleratorTable&& other) noexcept;
    AcceleratorTable& operator=(AcceleratorTable&& other) noexcept;

    [[nodiscard]] bool Create(std::span<const ACCEL> entries) noexcept;
    [[nodiscard]] HACCEL GetHandle() const noexcept { return table_; }
    void Reset() noexcept;

private:
    HACCEL table_ = nullptr;
};

struct FileDialogOptions {
    HWND owner = nullptr;
    const wchar_t* title = nullptr;
    const wchar_t* filter = nullptr;  // Double-NUL terminated Win32 filter.
    const wchar_t* default_extension = nullptr;
    std::wstring initial_path;
    DWORD flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    LPOFNHOOKPROC hook = nullptr;
    LPARAM custom_data = 0;
};

struct FileDialogResult {
    std::wstring path;
    DWORD extended_error = 0;
    bool accepted = false;

    [[nodiscard]] bool Cancelled() const noexcept {
        return !accepted && extended_error == 0;
    }
};

[[nodiscard]] FileDialogResult ShowOpenFileDialog(const FileDialogOptions& options);
[[nodiscard]] FileDialogResult ShowSaveFileDialog(const FileDialogOptions& options);

struct FolderDialogOptions {
    HWND owner = nullptr;
    const wchar_t* title = nullptr;
    UINT flags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_USENEWUI;
    BFFCALLBACK callback = nullptr;
    LPARAM custom_data = 0;
};

[[nodiscard]] FileDialogResult ShowFolderDialog(
    const FolderDialogOptions& options = {});

[[nodiscard]] bool SetClipboardText(HWND owner, std::wstring_view text) noexcept;
[[nodiscard]] std::wstring GetClipboardText(HWND owner = nullptr);

void EnableFileDrop(HWND window, bool enabled = true) noexcept;
[[nodiscard]] std::vector<std::wstring> ReadDroppedFiles(HDROP drop);

struct SavedWindowPlacement {
    WINDOWPLACEMENT placement{sizeof(WINDOWPLACEMENT)};
};

[[nodiscard]] bool CaptureWindowPlacement(HWND window,
                                          SavedWindowPlacement& output) noexcept;
[[nodiscard]] bool RestoreWindowPlacement(HWND window,
                                          const SavedWindowPlacement& saved,
                                          bool clamp_to_visible_work_area = true) noexcept;
[[nodiscard]] bool SaveWindowPlacementToRegistry(
    HKEY root, std::wstring_view subkey, std::wstring_view value_name,
    const SavedWindowPlacement& placement) noexcept;
[[nodiscard]] bool LoadWindowPlacementFromRegistry(
    HKEY root, std::wstring_view subkey, std::wstring_view value_name,
    SavedWindowPlacement& placement) noexcept;

class UiFont final {
public:
    UiFont() noexcept = default;
    ~UiFont() noexcept;
    UiFont(const UiFont&) = delete;
    UiFont& operator=(const UiFont&) = delete;
    UiFont(UiFont&& other) noexcept;
    UiFont& operator=(UiFont&& other) noexcept;

    [[nodiscard]] bool CreateMessageFont(UINT dpi) noexcept;
    [[nodiscard]] HFONT GetHandle() const noexcept { return font_; }
    void Reset() noexcept;

private:
    HFONT font_ = nullptr;
};

void SetControlFont(HWND control, HFONT font, bool redraw = true) noexcept;
[[nodiscard]] HWND FocusNextControl(HWND parent, HWND current,
                                    bool previous = false) noexcept;

}  // namespace mwtl
