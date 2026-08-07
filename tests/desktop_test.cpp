#include <mwtl/mwtl.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

bool accelerator_seen = false;

UINT_PTR CALLBACK CancelFileDialog(HWND dialog, UINT message, WPARAM, LPARAM) {
    if (message == WM_INITDIALOG) {
        ::PostMessageW(::GetParent(dialog), WM_COMMAND, IDCANCEL, 0);
    }
    return 0;
}

int CALLBACK CancelFolderDialog(HWND dialog, UINT message, LPARAM, LPARAM) {
    if (message == BFFM_INITIALIZED) {
        ::PostMessageW(dialog, WM_COMMAND, IDCANCEL, 0);
    }
    return 0;
}

class AcceleratorWindow final : public mwtl::Window<AcceleratorWindow> {
public:
    void BuildUI() {
        mwtl::Menu bar;
        mwtl::Menu commands;
        if (!bar.Create() || !commands.CreatePopup() ||
            !commands.AppendCommand(701, L"Verify\tF6") ||
            !bar.AppendSubmenu(std::move(commands), L"Test") ||
            !bar.AttachToWindow(GetHwnd())) {
            throw std::runtime_error("menu fixture failed");
        }
        const std::array<ACCEL, 1> entries{{ACCEL{FVIRTKEY, VK_F6, 701}}};
        if (!accelerators_.Create(entries)) {
            throw std::runtime_error("accelerator fixture failed");
        }
        SetAccelerators(accelerators_.GetHandle());
        ::SetTimer(GetHwnd(), 1, 10, nullptr);
        ::SetTimer(GetHwnd(), 2, 2000, nullptr);
    }

    mwtl::EventResult OnTimer(mwtl::TimerId id) {
        ::KillTimer(GetHwnd(), id.value);
        if (id.value == 1) {
            ::SetFocus(GetHwnd());
            ::PostMessageW(GetHwnd(), WM_KEYDOWN, VK_F6, 0);
        } else {
            (void)Close();
        }
        return mwtl::EventResult::Handled();
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) {
        if (event.control == nullptr && event.id.value == 701) {
            accelerator_seen = true;
            (void)Close();
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

private:
    mwtl::AcceleratorTable accelerators_;
};

bool VerifyClipboardRoundTrip() {
    const bool had_text = ::IsClipboardFormatAvailable(CF_UNICODETEXT) != FALSE;
    const std::wstring original_text = had_text ? mwtl::GetClipboardText() : L"";

    const bool wrote = mwtl::SetClipboardText(nullptr, L"mwtl clipboard round trip");
    const bool matched = wrote &&
        mwtl::GetClipboardText() == L"mwtl clipboard round trip";

    bool restored = had_text && mwtl::SetClipboardText(nullptr, original_text);
    if (!had_text && ::OpenClipboard(nullptr) != FALSE) {
        restored = ::EmptyClipboard() != FALSE;
        ::CloseClipboard();
    }
    // Restoration is best-effort: another desktop process can legitimately take
    // clipboard ownership between the verification and cleanup operations.
    (void)restored;
    return matched;
}

bool VerifyDroppedFiles() {
    constexpr wchar_t paths[] = L"C:\\first.txt\0D:\\folder\\second.txt\0\0";
    constexpr SIZE_T path_bytes = sizeof(paths);
    HGLOBAL memory = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
                                    sizeof(DROPFILES) + path_bytes);
    if (memory == nullptr) return false;
    auto* drop = static_cast<DROPFILES*>(::GlobalLock(memory));
    if (drop == nullptr) { ::GlobalFree(memory); return false; }
    drop->pFiles = sizeof(DROPFILES);
    drop->fWide = TRUE;
    std::memcpy(reinterpret_cast<BYTE*>(drop) + drop->pFiles, paths, path_bytes);
    ::GlobalUnlock(memory);

    const auto files = mwtl::ReadDroppedFiles(reinterpret_cast<HDROP>(memory));
    return files.size() == 2 && files[0] == L"C:\\first.txt" &&
           files[1] == L"D:\\folder\\second.txt";
}

}  // namespace

int main(int argc, char** argv) {
    const bool clipboard_only = argc == 2 && std::string_view(argv[1]) == "--clipboard-only";
    const bool skip_clipboard = argc == 2 && std::string_view(argv[1]) == "--skip-clipboard";
    if (clipboard_only) return VerifyClipboardRoundTrip() ? EXIT_SUCCESS : 6;

    mwtl::UiFont font;
    if (!font.CreateMessageFont(144) || font.GetHandle() == nullptr) return 1;

    constexpr wchar_t key[] = L"Software\\mwtl\\Tests\\DesktopTest";
    constexpr wchar_t value[] = L"Placement";
    mwtl::SavedWindowPlacement saved{};
    saved.placement.flags = WPF_ASYNCWINDOWPLACEMENT;
    saved.placement.showCmd = SW_SHOWNORMAL;
    saved.placement.rcNormalPosition = {10, 20, 410, 320};
    if (!mwtl::SaveWindowPlacementToRegistry(HKEY_CURRENT_USER, key, value, saved)) return 2;
    mwtl::SavedWindowPlacement loaded{};
    const bool loaded_ok = mwtl::LoadWindowPlacementFromRegistry(
        HKEY_CURRENT_USER, key, value, loaded);
    ::RegDeleteTreeW(HKEY_CURRENT_USER, key);
    if (!loaded_ok || loaded.placement.rcNormalPosition.left != 10 ||
        loaded.placement.rcNormalPosition.bottom != 320) return 3;

    mwtl::FileDialogOptions file_options{};
    file_options.title = L"mwtl automated cancellation test";
    file_options.hook = CancelFileDialog;
    if (!mwtl::ShowOpenFileDialog(file_options).Cancelled()) return 4;

    mwtl::FolderDialogOptions folder_options{};
    folder_options.title = L"mwtl automated cancellation test";
    folder_options.callback = CancelFolderDialog;
    if (!mwtl::ShowFolderDialog(folder_options).Cancelled()) return 5;

    if (!skip_clipboard && !VerifyClipboardRoundTrip()) return 6;
    if (!VerifyDroppedFiles()) return 7;

    const int accelerator_result = mwtl::Application(::GetModuleHandleW(nullptr))
        .Run<AcceleratorWindow>(SW_SHOWNOACTIVATE);
    if (accelerator_result != 0 || !accelerator_seen) return 8;

    return EXIT_SUCCESS;
}
