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
    bool Create(int width, int height, UINT flags = ILC_COLOR32 | ILC_MASK, int initial = 4, int grow = 4) noexcept;
    int AddIcon(HICON icon) noexcept;
    int GetCount() const noexcept;
    HIMAGELIST GetHandle() const noexcept { return handle_; }
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
    bool Create(HWND owner);
    bool AddTool(HWND tool, std::wstring_view text);
    HWND GetHwnd() const noexcept { return window_; }
    void Destroy() noexcept;
private:
    HWND window_ = nullptr;
    HWND owner_ = nullptr;
    std::vector<std::unique_ptr<std::wstring>> texts_;
};

struct TaskDialogResult { HRESULT status = E_FAIL; int button = 0; int radio_button = 0; bool verification_checked = false; explicit operator bool() const noexcept { return SUCCEEDED(status); } };
TaskDialogResult ShowTaskDialog(HWND owner, std::wstring_view title, std::wstring_view instruction, std::wstring_view content, TASKDIALOG_COMMON_BUTTON_FLAGS buttons = TDCBF_OK_BUTTON) noexcept;

bool InitializeFlatScrollBars(HWND window) noexcept;
bool UninitializeFlatScrollBars(HWND window) noexcept;

}  // namespace mwtl
