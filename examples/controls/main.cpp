#include <mwtl/mwtl.h>

#include <chrono>
#include <cstdlib>
#include <stdexcept>
#include <string>

using namespace std::chrono_literals;
using mwtl::operator""_dip;

class ControlsWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        SetTitle(L"Modern native controls");

        const bool created =
            heading_.Create(
                *this, kHeading, L"Native controls, modern C++20 ownership",
                {{28.0_dip, 22.0_dip}, {540.0_dip, 32.0_dip}}) &&
            name_label_.Create(
                *this, kNameLabel, L"Your name",
                {{28.0_dip, 66.0_dip}, {180.0_dip, 24.0_dip}}) &&
            name_.Create(
                *this, kName, L"mwtl developer",
                {{28.0_dip, 92.0_dip}, {360.0_dip, 34.0_dip}}) &&
            greet_.Create(
                *this, kGreet, L"Say hello",
                {{408.0_dip, 92.0_dip}, {160.0_dip, 34.0_dip}}) &&
            enabled_.Create(
                *this, kEnabled, L"Keep the native button enabled",
                {{28.0_dip, 144.0_dip}, {280.0_dip, 28.0_dip}}) &&
            accent_.Create(
                *this, kAccent,
                {{328.0_dip, 140.0_dip}, {240.0_dip, 180.0_dip}}) &&
            progress_.Create(
                *this, kProgress,
                {{28.0_dip, 196.0_dip}, {540.0_dip, 22.0_dip}}) &&
            status_.Create(
                *this, kStatus, L"Ready — the UI remains native HWNDs",
                {{28.0_dip, 242.0_dip}, {540.0_dip, 50.0_dip}}) &&
            choices_.Create(
                *this, kChoices, L"Choice controls",
                {{600.0_dip, 22.0_dip}, {300.0_dip, 154.0_dip}}) &&
            sky_.Create(
                *this, kSky, L"Sky blue",
                {{620.0_dip, 54.0_dip}, {130.0_dip, 28.0_dip}}) &&
            cosmos_.Create(
                *this, kCosmos, L"Cosmic violet",
                {{620.0_dip, 88.0_dip}, {150.0_dip, 28.0_dip}}) &&
            items_.Create(
                *this, kItems,
                {{600.0_dip, 194.0_dip}, {300.0_dip, 142.0_dip}}) &&
            volume_.Create(
                *this, kVolume,
                {{600.0_dip, 360.0_dip}, {300.0_dip, 42.0_dip}});
        if (!created || !heartbeat_.Start(*this, kHeartbeat, 1s)) {
            throw std::runtime_error("Could not create the controls example");
        }
        enabled_.SetChecked(true);
        static_cast<void>(accent_.AddItem(L"Sunrise orange"));
        static_cast<void>(accent_.AddItem(L"Geometry blue"));
        static_cast<void>(accent_.AddItem(L"Optimistic green"));
        static_cast<void>(accent_.SetSelection(0));
        progress_.SetRange(0, 100);
        progress_.SetValue(18);
        sky_.SetChecked(true);
        static_cast<void>(items_.AddItem(L"Label + text input"));
        static_cast<void>(items_.AddItem(L"Buttons and choices"));
        static_cast<void>(items_.AddItem(L"Lists and progress"));
        static_cast<void>(items_.SetSelection(0));
        volume_.SetRange(0, 100);
        volume_.SetValue(65);
        name_.SelectAll();
        name_.Focus();
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (event.IsClicked(greet_)) {
            static_cast<void>(status_.SetText(
                L"Hello, " + name_.GetText() +
                L"! Button notification received."));
        } else if (event.IsClicked(enabled_)) {
            greet_.SetEnabled(enabled_.IsChecked());
        } else if (event.id == kAccent && event.notification == CBN_SELCHANGE) {
            static_cast<void>(status_.SetText(
                L"ComboBox selection changed — still a native notification."));
        } else if (event.IsClicked(sky_) || event.IsClicked(cosmos_)) {
            static_cast<void>(status_.SetText(L"RadioButton choice changed."));
        } else if (event.id == kItems && event.notification == LBN_SELCHANGE) {
            static_cast<void>(status_.SetText(L"ListBox selection changed."));
        } else {
            return mwtl::EventResult::Propagate();
        }
        return mwtl::EventResult::Handled();
    }

    mwtl::EventResult OnTimer(mwtl::TimerId id) override {
        if (id == kHeartbeat) {
            ++ticks_;
            progress_.SetValue(static_cast<int>((ticks_ * 9) % 101));
            SetTitle(L"Modern native controls — heartbeat " +
                     std::to_wstring(ticks_));
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

    mwtl::EventResult OnMessage(const mwtl::WindowMessage& message) override {
        if (message.id == WM_HSCROLL &&
            reinterpret_cast<HWND>(message.lparam) == volume_.GetHwnd()) {
            progress_.SetValue(volume_.GetValue());
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
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
    static constexpr mwtl::ControlId kChoices{108};
    static constexpr mwtl::ControlId kSky{109};
    static constexpr mwtl::ControlId kCosmos{110};
    static constexpr mwtl::ControlId kItems{111};
    static constexpr mwtl::ControlId kVolume{112};
    static constexpr mwtl::TimerId kHeartbeat{1};

    mwtl::Label heading_;
    mwtl::Label name_label_;
    mwtl::TextBox name_;
    mwtl::Button greet_;
    mwtl::CheckBox enabled_;
    mwtl::ComboBox accent_;
    mwtl::ProgressBar progress_;
    mwtl::Label status_;
    mwtl::GroupBox choices_;
    mwtl::RadioButton sky_;
    mwtl::RadioButton cosmos_;
    mwtl::ListBox items_;
    mwtl::Slider volume_;
    mwtl::UiTimer heartbeat_;
    unsigned ticks_ = 0;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<ControlsWindow>(
        instance,
        show_command,
        {
            .title = L"Modern native controls",
            .initial_bounds = {{0.0_dip, 0.0_dip}, {960.0_dip, 500.0_dip}},
            .use_default_bounds = false,
        });
}
