#include <mwtl/mwtl.h>

#include <chrono>
#include <cstdlib>
#include <stdexcept>
#include <string>

using namespace std::chrono_literals;
using mwtl::operator""_dip;

class ControlsWindow final : public mwtl::Window<ControlsWindow> {
public:
    void BuildUI() {
        SetTitle(L"Modern native controls");

        const HWND parent = GetHwnd();
        const bool created =
            heading_.Create(
                parent, kHeading, L"Native controls, modern C++20 ownership",
                {{28.0_dip, 22.0_dip}, {540.0_dip, 32.0_dip}}) &&
            name_label_.Create(
                parent, kNameLabel, L"Your name",
                {{28.0_dip, 66.0_dip}, {180.0_dip, 24.0_dip}}) &&
            name_.Create(
                parent, kName, L"mwtl developer",
                {{28.0_dip, 92.0_dip}, {360.0_dip, 34.0_dip}}) &&
            greet_.Create(
                parent, kGreet, L"Say hello",
                {{408.0_dip, 92.0_dip}, {160.0_dip, 34.0_dip}}) &&
            enabled_.Create(
                parent, kEnabled, L"Keep the native button enabled",
                {{28.0_dip, 144.0_dip}, {280.0_dip, 28.0_dip}}) &&
            accent_.Create(
                parent, kAccent,
                {{328.0_dip, 140.0_dip}, {240.0_dip, 180.0_dip}}) &&
            progress_.Create(
                parent, kProgress,
                {{28.0_dip, 196.0_dip}, {540.0_dip, 22.0_dip}}) &&
            status_.Create(
                parent, kStatus, L"Ready — the UI remains native HWNDs",
                {{28.0_dip, 242.0_dip}, {540.0_dip, 50.0_dip}});
        if (!created || !heartbeat_.Start(parent, kHeartbeat, 1s)) {
            throw std::runtime_error("Could not create the controls example");
        }
        enabled_.SetChecked(true);
        static_cast<void>(accent_.AddItem(L"Sunrise orange"));
        static_cast<void>(accent_.AddItem(L"Geometry blue"));
        static_cast<void>(accent_.AddItem(L"Optimistic green"));
        static_cast<void>(accent_.SetSelection(0));
        progress_.SetRange(0, 100);
        progress_.SetValue(18);
        name_.SelectAll();
        name_.Focus();
    }

    void OnCommand(const mwtl::CommandEvent& event) {
        if (event.id == kGreet && event.notification == BN_CLICKED) {
            static_cast<void>(status_.SetText(
                L"Hello, " + name_.GetText() +
                L"! Button notification received."));
        } else if (event.id == kEnabled && event.notification == BN_CLICKED) {
            greet_.SetEnabled(enabled_.IsChecked());
        } else if (event.id == kAccent && event.notification == CBN_SELCHANGE) {
            static_cast<void>(status_.SetText(
                L"ComboBox selection changed — still a native notification."));
        }
    }

    void OnTimer(mwtl::TimerId id) {
        if (id == kHeartbeat) {
            ++ticks_;
            progress_.SetValue(static_cast<int>((ticks_ * 9) % 101));
            SetTitle(L"Modern native controls — heartbeat " +
                     std::to_wstring(ticks_));
        }
    }

private:
    static constexpr mwtl::ControlId kHeading{100};
    static constexpr mwtl::ControlId kNameLabel{101};
    static constexpr mwtl::ControlId kName{102};
    static constexpr mwtl::ControlId kGreet{103};
    static constexpr mwtl::ControlId kEnabled{104};
    static constexpr mwtl::ControlId kAccent{105};
    static constexpr mwtl::ControlId kProgress{106};
    static constexpr mwtl::ControlId kStatus{107};
    static constexpr mwtl::TimerId kHeartbeat{1};

    mwtl::Label heading_;
    mwtl::Label name_label_;
    mwtl::TextBox name_;
    mwtl::Button greet_;
    mwtl::CheckBox enabled_;
    mwtl::ComboBox accent_;
    mwtl::ProgressBar progress_;
    mwtl::Label status_;
    mwtl::UiTimer heartbeat_;
    unsigned ticks_ = 0;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<ControlsWindow>(
        instance,
        show_command,
        {
            .title = L"Modern native controls",
            .initial_bounds = {{0.0_dip, 0.0_dip}, {960.0_dip, 520.0_dip}},
            .use_default_bounds = false,
        });
}
