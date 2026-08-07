#include <mwtl/mwtl.h>

#include "hot_corner_model.h"

#include <shellapi.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>

using namespace mwtl;
using namespace std::chrono_literals;

namespace {

constexpr ControlId kEnabled{100};
constexpr ControlId kMonitor{101};
constexpr ControlId kFirstAction{110};
constexpr ControlId kDwell{120};
constexpr ControlId kTolerance{121};
constexpr ControlId kFullscreen{122};
constexpr ControlId kStatus{130};
constexpr TimerId kPoll{1};
constexpr TimerId kSelfTest{2};
constexpr UINT kToggleCommand = 200;
constexpr UINT kExitCommand = 201;
constexpr UINT kTrayMessage = WM_APP + 42;
constexpr wchar_t kRegistryKey[] = L"Software\\mwtl\\Examples\\HotCorners";
constexpr std::array<std::uint32_t, 4> kDwellValues{200, 350, 500, 750};
constexpr std::array<LONG, 4> kToleranceValues{1, 2, 4, 8};

bool g_test_mode = false;
bool g_self_test = false;

const wchar_t* ActionName(hot_corners::Action action) noexcept {
    switch (action) {
    case hot_corners::Action::none: return L"Disabled";
    case hot_corners::Action::task_view: return L"Task View";
    case hot_corners::Action::notifications: return L"Notifications";
    case hot_corners::Action::start: return L"Start";
    case hot_corners::Action::show_desktop: return L"Show Desktop";
    }
    return L"Disabled";
}

void SendAction(hot_corners::Action action) noexcept {
    if (g_test_mode || action == hot_corners::Action::none) return;
    WORD key = 0;
    switch (action) {
    case hot_corners::Action::task_view: key = VK_TAB; break;
    case hot_corners::Action::notifications: key = 'N'; break;
    case hot_corners::Action::start: break;
    case hot_corners::Action::show_desktop: key = 'D'; break;
    case hot_corners::Action::none: return;
    }
    std::array<INPUT, 4> inputs{};
    UINT count = 0;
    inputs[count].type = INPUT_KEYBOARD;
    inputs[count++].ki.wVk = VK_LWIN;
    if (key != 0) {
        inputs[count].type = INPUT_KEYBOARD;
        inputs[count++].ki.wVk = key;
        inputs[count].type = INPUT_KEYBOARD;
        inputs[count].ki.wVk = key;
        inputs[count++].ki.dwFlags = KEYEVENTF_KEYUP;
    }
    inputs[count].type = INPUT_KEYBOARD;
    inputs[count].ki.wVk = VK_LWIN;
    inputs[count++].ki.dwFlags = KEYEVENTF_KEYUP;
    ::SendInput(count, inputs.data(), sizeof(INPUT));
}

BOOL CALLBACK CollectMonitor(HMONITOR monitor, HDC, LPRECT, LPARAM data) {
    auto& areas = *reinterpret_cast<std::vector<RECT>*>(data);
    MONITORINFO info{sizeof(info)};
    if (::GetMonitorInfoW(monitor, &info) != FALSE) areas.push_back(info.rcMonitor);
    return TRUE;
}

bool IsFullscreenBusy() noexcept {
    QUERY_USER_NOTIFICATION_STATE state = QUNS_NOT_PRESENT;
    if (FAILED(::SHQueryUserNotificationState(&state))) return false;
    return state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_BUSY ||
           state == QUNS_PRESENTATION_MODE;
}

class HotCornersWindow final : public Window<HotCornersWindow> {
public:
    void BuildUI() {
        SetTitle(L"mwtl Hot Corners - x64 multi-monitor reference");
        BuildMenu();
        CreateControls();
        RefreshMonitors();
        LoadSettings();
        PopulateSettingsControls();
        ApplyFont(GetDpiContext().GetDpi());
        AddTrayIcon();
        if (!poll_.Start(*this, kPoll, 30ms)) throw std::runtime_error("poll timer creation failed");
        if (g_self_test && ::SetTimer(GetHwnd(), kSelfTest.value, 100, nullptr) == 0)
            throw std::runtime_error("self-test timer creation failed");
        SavedWindowPlacement saved{};
        if (LoadWindowPlacementFromRegistry(HKEY_CURRENT_USER, kRegistryKey, L"MainWindow", saved))
            (void)RestoreWindowPlacement(GetHwnd(), saved);
    }

    EventResult OnCommand(const CommandEvent& event) {
        if (event.IsClicked(enabled_)) {
            settings_.enabled = enabled_.IsChecked();
            tracker_.Reset(); SaveSettings(); UpdateStatus();
            return EventResult::Handled();
        }
        if (event.IsClicked(fullscreen_)) {
            settings_.pause_for_fullscreen = fullscreen_.IsChecked();
            SaveSettings(); UpdateStatus();
            return EventResult::Handled();
        }
        if (event.control == monitor_.GetHwnd() && event.notification == CBN_SELCHANGE) {
            StoreVisibleMonitor();
            shown_monitor_ = static_cast<std::size_t>((std::max)(0, monitor_.GetSelection()));
            LoadVisibleMonitor();
            return EventResult::Handled();
        }
        for (const auto& combo : actions_) {
            if (event.control == combo.GetHwnd() && event.notification == CBN_SELCHANGE) {
                StoreVisibleMonitor(); SaveSettings(); return EventResult::Handled();
            }
        }
        if (event.control == dwell_.GetHwnd() && event.notification == CBN_SELCHANGE) {
            settings_.dwell_ms = kDwellValues[static_cast<std::size_t>((std::max)(0, dwell_.GetSelection()))];
            tracker_.Reset(); SaveSettings(); UpdateStatus(); return EventResult::Handled();
        }
        if (event.control == tolerance_.GetHwnd() && event.notification == CBN_SELCHANGE) {
            settings_.tolerance = kToleranceValues[static_cast<std::size_t>((std::max)(0, tolerance_.GetSelection()))];
            tracker_.Reset(); SaveSettings(); UpdateStatus(); return EventResult::Handled();
        }
        if (event.control == nullptr && event.id.value == static_cast<int>(kToggleCommand)) {
            manual_paused_ = !manual_paused_; tracker_.Reset(); UpdateStatus(); return EventResult::Handled();
        }
        if (event.control == nullptr && event.id.value == static_cast<int>(kExitCommand)) {
            (void)Close(); return EventResult::Handled();
        }
        return EventResult::Propagate();
    }

    EventResult OnClose() {
        StoreVisibleMonitor(); SaveSettings(); RemoveTrayIcon();
        SavedWindowPlacement saved{};
        if (CaptureWindowPlacement(GetHwnd(), saved))
            (void)SaveWindowPlacementToRegistry(HKEY_CURRENT_USER, kRegistryKey, L"MainWindow", saved);
        return EventResult::Propagate();
    }

    EventResult OnTimer(TimerId id) {
        if (id == kSelfTest) {
            ::KillTimer(GetHwnd(), kSelfTest.value);
            Activate({0, hot_corners::Corner::top_left});
            (void)Close();
            return EventResult::Handled();
        }
        if (id != kPoll) return EventResult::Propagate();
        const bool fullscreen_paused = settings_.pause_for_fullscreen && IsFullscreenBusy();
        if (!settings_.enabled || manual_paused_ || fullscreen_paused) {
            tracker_.Reset();
            if (fullscreen_paused != last_fullscreen_paused_) { last_fullscreen_paused_ = fullscreen_paused; UpdateStatus(); }
            return EventResult::Handled();
        }
        if (last_fullscreen_paused_) { last_fullscreen_paused_ = false; UpdateStatus(); }
        POINT cursor{};
        if (::GetCursorPos(&cursor) == FALSE) return EventResult::Handled();
        const auto fired = tracker_.Update(
            hot_corners::Detect(cursor, monitors_, settings_.tolerance),
            ::GetTickCount64(), settings_.dwell_ms);
        if (fired) Activate(*fired);
        return EventResult::Handled();
    }

    EventResult OnMessage(const WindowMessage& event) {
        if (event.id == WM_DISPLAYCHANGE || event.id == WM_SETTINGCHANGE) {
            StoreVisibleMonitor(); RefreshMonitors(); PopulateMonitorList(); LoadVisibleMonitor();
            return EventResult::Handled();
        }
        if (event.id == kTrayMessage) {
            if (event.lparam == WM_LBUTTONDBLCLK) {
                manual_paused_ = !manual_paused_; tracker_.Reset(); UpdateStatus();
            } else if (event.lparam == WM_RBUTTONUP) {
                ShowTrayMenu();
            }
            return EventResult::Handled();
        }
        return EventResult::Propagate();
    }

    EventResult OnDpiChanged(const DpiChangedEvent& event) { ApplyFont(event.dpi_x); return EventResult::Propagate(); }

private:
    void BuildMenu() {
        Menu bar; Menu app;
        if (!bar.Create() || !app.CreatePopup() ||
            !app.AppendCommand(kToggleCommand, L"&Pause / Resume\tCtrl+E") ||
            !app.AppendSeparator() || !app.AppendCommand(kExitCommand, L"E&xit\tCtrl+Q") ||
            !bar.AppendSubmenu(std::move(app), L"&Hot Corners") || !bar.AttachToWindow(GetHwnd()))
            throw std::runtime_error("menu creation failed");
        const std::array<ACCEL, 2> keys{{
            ACCEL{FVIRTKEY | FCONTROL, 'E', static_cast<WORD>(kToggleCommand)},
            ACCEL{FVIRTKEY | FCONTROL, 'Q', static_cast<WORD>(kExitCommand)}}};
        if (!accelerators_.Create(keys)) throw std::runtime_error("accelerator creation failed");
        SetAccelerators(accelerators_.GetHandle());
    }

    void CreateControls() {
        auto require = [](bool ok) { if (!ok) throw std::runtime_error("control creation failed"); };
        require(enabled_.Create(*this, kEnabled, L"Enabled", {{16_dip, 16_dip}, {120_dip, 24_dip}}));
        require(fullscreen_.Create(*this, kFullscreen, L"Pause for fullscreen apps", {{150_dip, 16_dip}, {220_dip, 24_dip}}));
        require(monitor_.Create(*this, kMonitor, {{16_dip, 56_dip}, {220_dip, 180_dip}}));
        constexpr std::array<const wchar_t*, 4> corners{L"Top left", L"Top right", L"Bottom left", L"Bottom right"};
        for (std::size_t i = 0; i < 4; ++i) {
            require(corner_labels_[i].Create(*this, {static_cast<int>(140 + i)}, corners[i],
                {{260_dip, Dip{56.0f + static_cast<float>(i) * 42.0f}}, {110_dip, 24_dip}}));
            require(actions_[i].Create(*this, {static_cast<int>(kFirstAction.value + i)},
                {{380_dip, Dip{52.0f + static_cast<float>(i) * 42.0f}}, {190_dip, 180_dip}}));
            for (int action = 0; action <= 4; ++action)
                (void)actions_[i].AddItem(ActionName(static_cast<hot_corners::Action>(action)));
        }
        require(dwell_label_.Create(*this, {150}, L"Dwell", {{16_dip, 246_dip}, {80_dip, 24_dip}}));
        require(dwell_.Create(*this, kDwell, {{96_dip, 242_dip}, {140_dip, 150_dip}}));
        for (auto value : kDwellValues) (void)dwell_.AddItem(std::to_wstring(value) + L" ms");
        require(tolerance_label_.Create(*this, {151}, L"Tolerance", {{260_dip, 246_dip}, {100_dip, 24_dip}}));
        require(tolerance_.Create(*this, kTolerance, {{360_dip, 242_dip}, {140_dip, 150_dip}}));
        for (auto value : kToleranceValues) (void)tolerance_.AddItem(std::to_wstring(value) + L" px");
        require(status_.Create(*this, kStatus, L"Starting...", {{16_dip, 302_dip}, {650_dip, 48_dip}}));
    }

    void RefreshMonitors() {
        monitors_.clear();
        ::EnumDisplayMonitors(nullptr, nullptr, CollectMonitor, reinterpret_cast<LPARAM>(&monitors_));
        settings_.monitors.resize(monitors_.size());
        shown_monitor_ = (std::min)(shown_monitor_, monitors_.empty() ? std::size_t{0} : monitors_.size() - 1);
        tracker_.Reset();
    }

    void PopulateSettingsControls() {
        ::SendMessageW(enabled_.GetHwnd(), BM_SETCHECK, settings_.enabled ? BST_CHECKED : BST_UNCHECKED, 0);
        ::SendMessageW(fullscreen_.GetHwnd(), BM_SETCHECK, settings_.pause_for_fullscreen ? BST_CHECKED : BST_UNCHECKED, 0);
        PopulateMonitorList();
        const auto dwell = std::find(kDwellValues.begin(), kDwellValues.end(), settings_.dwell_ms);
        (void)dwell_.SetSelection(dwell == kDwellValues.end() ? 1 : static_cast<int>(dwell - kDwellValues.begin()));
        const auto tolerance = std::find(kToleranceValues.begin(), kToleranceValues.end(), settings_.tolerance);
        (void)tolerance_.SetSelection(tolerance == kToleranceValues.end() ? 1 : static_cast<int>(tolerance - kToleranceValues.begin()));
        LoadVisibleMonitor(); UpdateStatus();
    }

    void PopulateMonitorList() {
        ::SendMessageW(monitor_.GetHwnd(), CB_RESETCONTENT, 0, 0);
        for (std::size_t i = 0; i < monitors_.size(); ++i)
            (void)monitor_.AddItem(L"Monitor " + std::to_wstring(i + 1));
        if (!monitors_.empty()) (void)monitor_.SetSelection(static_cast<int>(shown_monitor_));
    }

    void LoadVisibleMonitor() {
        if (shown_monitor_ >= settings_.monitors.size()) return;
        for (std::size_t i = 0; i < 4; ++i)
            (void)actions_[i].SetSelection(static_cast<int>(settings_.monitors[shown_monitor_].corners[i]));
    }

    void StoreVisibleMonitor() {
        if (shown_monitor_ >= settings_.monitors.size()) return;
        for (std::size_t i = 0; i < 4; ++i) {
            const int selected = actions_[i].GetSelection();
            if (selected >= 0 && selected <= 4)
                settings_.monitors[shown_monitor_].corners[i] = static_cast<hot_corners::Action>(selected);
        }
    }

    void LoadSettings() {
        HKEY key = nullptr;
        if (::RegOpenKeyExW(HKEY_CURRENT_USER, kRegistryKey, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) return;
        auto close = wil::scope_exit([&] { ::RegCloseKey(key); });
        auto read = [&](const std::wstring& name, DWORD fallback) {
            DWORD value = fallback, bytes = sizeof(value), type = 0;
            return ::RegQueryValueExW(key, name.c_str(), nullptr, &type, reinterpret_cast<BYTE*>(&value), &bytes) == ERROR_SUCCESS && type == REG_DWORD ? value : fallback;
        };
        settings_.dwell_ms = read(L"DwellMs", settings_.dwell_ms);
        settings_.tolerance = static_cast<LONG>(read(L"Tolerance", settings_.tolerance));
        settings_.enabled = read(L"Enabled", 1) != 0;
        settings_.pause_for_fullscreen = read(L"PauseFullscreen", 1) != 0;
        for (std::size_t m = 0; m < settings_.monitors.size(); ++m)
            for (std::size_t c = 0; c < 4; ++c)
                settings_.monitors[m].corners[c] = static_cast<hot_corners::Action>(
                    (std::min<DWORD>)(4, read(L"Monitor" + std::to_wstring(m) + L"Corner" + std::to_wstring(c), static_cast<DWORD>(settings_.monitors[m].corners[c]))));
    }

    void SaveSettings() const {
        HKEY key = nullptr; DWORD disposition = 0;
        if (::RegCreateKeyExW(HKEY_CURRENT_USER, kRegistryKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &key, &disposition) != ERROR_SUCCESS) return;
        auto close = wil::scope_exit([&] { ::RegCloseKey(key); });
        auto write = [&](const std::wstring& name, DWORD value) {
            ::RegSetValueExW(key, name.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
        };
        write(L"DwellMs", settings_.dwell_ms); write(L"Tolerance", settings_.tolerance);
        write(L"Enabled", settings_.enabled); write(L"PauseFullscreen", settings_.pause_for_fullscreen);
        for (std::size_t m = 0; m < settings_.monitors.size(); ++m)
            for (std::size_t c = 0; c < 4; ++c)
                write(L"Monitor" + std::to_wstring(m) + L"Corner" + std::to_wstring(c), static_cast<DWORD>(settings_.monitors[m].corners[c]));
    }

    void Activate(const hot_corners::Hit& hit) {
        const auto action = hot_corners::ResolveAction(settings_, hit);
        (void)status_.SetText((g_test_mode ? L"TEST (input suppressed): " : L"") +
            std::wstring(L"Monitor ") + std::to_wstring(hit.monitor + 1) + L" - " + ActionName(action));
        SendAction(action);
    }

    void UpdateStatus() {
        std::wstring state = !settings_.enabled ? L"Disabled" : manual_paused_ ? L"Paused from tray/menu" :
            last_fullscreen_paused_ ? L"Paused for fullscreen app" : L"Watching";
        if (g_test_mode) state += L" (test mode: input suppressed)";
        (void)status_.SetText(state + L"; " + std::to_wstring(monitors_.size()) + L" monitor(s), " +
            std::to_wstring(settings_.dwell_ms) + L" ms / " + std::to_wstring(settings_.tolerance) + L" px");
    }

    void AddTrayIcon() {
        NOTIFYICONDATAW data{sizeof(data)}; data.hWnd = GetHwnd(); data.uID = 1;
        data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; data.uCallbackMessage = kTrayMessage;
        data.hIcon = ::LoadIconW(nullptr, IDI_APPLICATION);
        wcscpy_s(data.szTip, L"mwtl Hot Corners");
        tray_added_ = ::Shell_NotifyIconW(NIM_ADD, &data) != FALSE;
    }

    void RemoveTrayIcon() noexcept {
        if (!tray_added_) return;
        NOTIFYICONDATAW data{sizeof(data)}; data.hWnd = GetHwnd(); data.uID = 1;
        ::Shell_NotifyIconW(NIM_DELETE, &data); tray_added_ = false;
    }

    void ShowTrayMenu() {
        HMENU menu = ::CreatePopupMenu(); if (menu == nullptr) return;
        ::AppendMenuW(menu, MF_STRING, kToggleCommand, manual_paused_ ? L"Resume" : L"Pause");
        ::AppendMenuW(menu, MF_SEPARATOR, 0, nullptr); ::AppendMenuW(menu, MF_STRING, kExitCommand, L"Exit");
        POINT point{}; ::GetCursorPos(&point); ::SetForegroundWindow(GetHwnd());
        const UINT command = ::TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, point.x, point.y, 0, GetHwnd(), nullptr);
        ::DestroyMenu(menu); if (command != 0) ::PostMessageW(GetHwnd(), WM_COMMAND, command, 0);
    }

    void ApplyFont(UINT dpi) {
        if (!font_.CreateMessageFont(dpi)) return;
        for (HWND control : {enabled_.GetHwnd(), fullscreen_.GetHwnd(), monitor_.GetHwnd(), dwell_.GetHwnd(), tolerance_.GetHwnd(), status_.GetHwnd()})
            SetControlFont(control, font_.GetHandle());
        for (std::size_t i = 0; i < 4; ++i) { SetControlFont(corner_labels_[i].GetHwnd(), font_.GetHandle()); SetControlFont(actions_[i].GetHwnd(), font_.GetHandle()); }
    }

    CheckBox enabled_, fullscreen_;
    ComboBox monitor_, dwell_, tolerance_;
    std::array<Label, 4> corner_labels_;
    std::array<ComboBox, 4> actions_;
    Label dwell_label_, tolerance_label_, status_;
    UiTimer poll_;
    UiFont font_;
    AcceleratorTable accelerators_;
    std::vector<RECT> monitors_;
    hot_corners::Settings settings_;
    hot_corners::DwellTracker tracker_;
    std::size_t shown_monitor_ = 0;
    bool manual_paused_ = false, last_fullscreen_paused_ = false, tray_added_ = false;
};

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    g_self_test = wcsstr(::GetCommandLineW(), L"--self-test") != nullptr;
    g_test_mode = g_self_test || wcsstr(::GetCommandLineW(), L"--test-mode") != nullptr;
    WindowOptions options{};
    options.title = L"mwtl Hot Corners";
    options.initial_bounds = {{0_dip, 0_dip}, {700_dip, 410_dip}};
    options.use_default_bounds = false;
    return RunApplication<HotCornersWindow>(instance, show, options);
}
