#pragma once

#include <windows.h>
#include <commctrl.h>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace mwtl {

class ImageList final {
public:
    ImageList() noexcept = default;
    ~ImageList() noexcept;
    ImageList(const ImageList&) = delete;
    ImageList& operator=(const ImageList&) = delete;
    ImageList(ImageList&& other) noexcept;
    ImageList& operator=(ImageList&& other) noexcept;
    [[nodiscard]] bool Create(int width, int height, UINT flags = ILC_COLOR32 | ILC_MASK, int initial = 4, int grow = 4) noexcept;
    [[nodiscard]] int AddIcon(HICON icon) noexcept;
    [[nodiscard]] int GetCount() const noexcept;
    [[nodiscard]] HIMAGELIST GetHandle() const noexcept { return handle_; }
    void Reset() noexcept;
private:
    HIMAGELIST handle_ = nullptr;
};

class Tooltip final {
public:
    Tooltip() noexcept = default;
    ~Tooltip() noexcept;
    Tooltip(const Tooltip&) = delete;
    Tooltip& operator=(const Tooltip&) = delete;
    Tooltip(Tooltip&& other) noexcept;
    Tooltip& operator=(Tooltip&& other) noexcept;
    [[nodiscard]] bool Create(HWND owner);
    [[nodiscard]] bool AddTool(HWND tool, std::wstring_view text);
    [[nodiscard]] HWND GetHwnd() const noexcept { return window_; }
    void Destroy() noexcept;
private:
    HWND window_ = nullptr;
    HWND owner_ = nullptr;
    std::vector<std::unique_ptr<std::wstring>> texts_;
};

struct TaskDialogResult { HRESULT status = E_FAIL; int button = 0; int radio_button = 0; bool verification_checked = false; [[nodiscard]] explicit operator bool() const noexcept { return SUCCEEDED(status); } };
[[nodiscard]] TaskDialogResult ShowTaskDialog(HWND owner, std::wstring_view title, std::wstring_view instruction, std::wstring_view content, TASKDIALOG_COMMON_BUTTON_FLAGS buttons = TDCBF_OK_BUTTON) noexcept;

[[nodiscard]] bool InitializeFlatScrollBars(HWND window) noexcept;
[[nodiscard]] bool UninitializeFlatScrollBars(HWND window) noexcept;

}  // namespace mwtl
