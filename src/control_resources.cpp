#include <mwtl/control_resources.h>

#include <wil/resource.h>

#include <string>
#include <utility>

namespace mwtl {

ImageList::~ImageList() noexcept { Reset(); }
ImageList::ImageList(ImageList&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}
ImageList& ImageList::operator=(ImageList&& other) noexcept { if (this != &other) { Reset(); handle_ = std::exchange(other.handle_, nullptr); } return *this; }
bool ImageList::Create(int width, int height, UINT flags, int initial, int grow) noexcept { Reset(); handle_ = ImageList_Create(width, height, flags, initial, grow); return handle_ != nullptr; }
int ImageList::AddIcon(HICON icon) noexcept { return handle_ != nullptr && icon != nullptr ? ImageList_ReplaceIcon(handle_, -1, icon) : -1; }
int ImageList::GetCount() const noexcept { return handle_ != nullptr ? ImageList_GetImageCount(handle_) : 0; }
void ImageList::Reset() noexcept { if (handle_ != nullptr) ImageList_Destroy(handle_); handle_ = nullptr; }

Tooltip::~Tooltip() noexcept { Destroy(); }
Tooltip::Tooltip(Tooltip&& other) noexcept : window_(std::exchange(other.window_, nullptr)), owner_(std::exchange(other.owner_, nullptr)), texts_(std::move(other.texts_)) {}
Tooltip& Tooltip::operator=(Tooltip&& other) noexcept { if (this != &other) { Destroy(); window_ = std::exchange(other.window_, nullptr); owner_ = std::exchange(other.owner_, nullptr); texts_ = std::move(other.texts_); } return *this; }
bool Tooltip::Create(HWND owner) { Destroy(); if (owner == nullptr) { ::SetLastError(ERROR_INVALID_WINDOW_HANDLE); return false; } INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_WIN95_CLASSES}; if (::InitCommonControlsEx(&controls) == FALSE) return false; window_ = ::CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr, WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, owner, nullptr, reinterpret_cast<HINSTANCE>(::GetWindowLongPtrW(owner, GWLP_HINSTANCE)), nullptr); if (window_ == nullptr) return false; owner_ = owner; ::SetWindowPos(window_, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); return true; }
bool Tooltip::AddTool(HWND tool, std::wstring_view text) { if (window_ == nullptr || tool == nullptr || ::GetParent(tool) != owner_) return false; texts_.push_back(std::make_unique<std::wstring>(text)); TOOLINFOW info{}; info.cbSize = sizeof(info); info.uFlags = TTF_IDISHWND | TTF_SUBCLASS; info.hwnd = owner_; info.uId = reinterpret_cast<UINT_PTR>(tool); info.lpszText = texts_.back()->data(); if (::SendMessageW(window_, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&info)) == FALSE) { texts_.pop_back(); return false; } return true; }
void Tooltip::Destroy() noexcept { if (window_ != nullptr && ::IsWindow(window_) != FALSE) ::DestroyWindow(window_); window_ = nullptr; owner_ = nullptr; texts_.clear(); }

TaskDialogResult ShowTaskDialog(HWND owner, std::wstring_view title, std::wstring_view instruction, std::wstring_view content, TASKDIALOG_COMMON_BUTTON_FLAGS buttons) noexcept {
    TaskDialogResult result;
    try {
        using TaskDialogIndirectFunction = HRESULT(WINAPI*)(const TASKDIALOGCONFIG*, int*, int*, BOOL*);
        const HMODULE common_controls = ::LoadLibraryW(L"comctl32.dll");
        if (common_controls == nullptr) {
            result.status = HRESULT_FROM_WIN32(::GetLastError());
            return result;
        }
        const auto release_common_controls = wil::scope_exit(
            [common_controls]() noexcept { ::FreeLibrary(common_controls); });

        const auto task_dialog = reinterpret_cast<TaskDialogIndirectFunction>(
            ::GetProcAddress(common_controls, "TaskDialogIndirect"));
        if (task_dialog == nullptr) {
            const DWORD error = ::GetLastError();
            result.status = HRESULT_FROM_WIN32(error);
            return result;
        }

        const std::wstring title_text{title};
        const std::wstring instruction_text{instruction};
        const std::wstring content_text{content};
        TASKDIALOGCONFIG config{};
        config.cbSize = sizeof(config);
        config.hwndParent = owner;
        config.dwCommonButtons = buttons;
        config.pszWindowTitle = title_text.c_str();
        config.pszMainInstruction = instruction_text.c_str();
        config.pszContent = content_text.c_str();
        BOOL checked = FALSE;
        result.status = task_dialog(&config, &result.button, &result.radio_button, &checked);
        result.verification_checked = checked != FALSE;
    } catch (...) {
        result.status = E_OUTOFMEMORY;
    }
    return result;
}

bool InitializeFlatScrollBars(HWND window) noexcept { return window != nullptr && ::InitializeFlatSB(window) != FALSE; }
bool UninitializeFlatScrollBars(HWND window) noexcept { return window != nullptr && ::UninitializeFlatSB(window) != FALSE; }

}  // namespace mwtl
