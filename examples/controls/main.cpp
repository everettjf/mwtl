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
        mwtl::ControlHost ui{*this};
        ui.Add(heading_, kHeading, L"Native controls, modern C++20 ownership");
        ui.Add(name_label_, kNameLabel, L"Your name");
        ui.Add(name_, kName, L"mwtl developer");
        ui.Add(greet_, kGreet, L"Say hello");
        ui.Add(enabled_, kEnabled, L"Keep the native button enabled");
        ui.Add(accent_, kAccent);
        ui.Add(progress_, kProgress);
        ui.Add(status_, kStatus, L"Ready — the UI remains native HWNDs");
        ui.Add(choices_, kChoices, L"Choice controls");
        ui.Add(sky_, kSky, L"Sky blue");
        ui.Add(cosmos_, kCosmos, L"Cosmic violet");
        ui.Add(items_, kItems);
        ui.Add(volume_, kVolume);
        mwtl::Must(
            heartbeat_.Start(*this, kHeartbeat, 1s), "start heartbeat");
        enabled_.SetChecked(true);
        mwtl::Must(mwtl::AddItems(
            accent_, {L"Sunrise orange", L"Geometry blue", L"Optimistic green"}),
            "populate accent choices");
        static_cast<void>(accent_.SetSelection(0));
        progress_.SetRange(0, 100);
        progress_.SetValue(18);
        sky_.SetChecked(true);
        mwtl::Must(mwtl::AddItems(
            items_, {L"Label + text input", L"Buttons and choices", L"Lists and progress"}),
            "populate control topics");
        static_cast<void>(items_.SetSelection(0));
        volume_.SetRange(0, 100);
        volume_.SetValue(65);
        name_.SelectAll();
        name_.Focus();

        UseLayout(
            mwtl::Row()
                .Margin(28.0_dip)
                .Gap(32.0_dip)
                .Add(
                    mwtl::Column()
                        .Gap(10.0_dip)
                        .Add(heading_, mwtl::Fixed(32.0_dip))
                        .Add(name_label_, mwtl::Fixed(24.0_dip))
                        .Add(
                            mwtl::Row()
                                .Gap(20.0_dip)
                                .Add(name_, mwtl::Stretch(1.0f, 180.0_dip))
                                .Add(greet_, mwtl::Fixed(160.0_dip)),
                            mwtl::Fixed(34.0_dip))
                        .Add(
                            mwtl::Row()
                                .Gap(20.0_dip)
                                .Add(enabled_, mwtl::Stretch())
                                .Add(accent_, mwtl::Fixed(240.0_dip), {
                                    .native_size = mwtl::SizeDip{0.0_dip, 180.0_dip},
                                }),
                            mwtl::Fixed(34.0_dip))
                        .Add(progress_, mwtl::Fixed(22.0_dip))
                        .Add(status_, mwtl::Stretch(1.0f, 50.0_dip)),
                    mwtl::Stretch(1.0f, 540.0_dip))
                .Add(
                    mwtl::Column()
                        .Gap(18.0_dip)
                        .Add(
                            mwtl::Overlay()
                                .Add(choices_)
                                .Add(
                                    mwtl::Column()
                                        .Margin({20.0_dip, 32.0_dip, 20.0_dip, 18.0_dip})
                                        .Gap(6.0_dip)
                                        .Add(sky_, mwtl::Fixed(28.0_dip))
                                        .Add(cosmos_, mwtl::Fixed(28.0_dip))),
                            mwtl::Fixed(154.0_dip))
                        .Add(items_, mwtl::Stretch(1.0f, 100.0_dip))
                        .Add(volume_, mwtl::Fixed(42.0_dip)),
                    mwtl::Fixed(300.0_dip)));
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (event.IsClicked(greet_)) {
            static_cast<void>(status_.SetText(
                L"Hello, " + name_.GetText() +
                L"! Button notification received."));
        } else if (event.IsClicked(enabled_)) {
            greet_.SetEnabled(enabled_.IsChecked());
        } else if (event.Is(accent_, CBN_SELCHANGE)) {
            static_cast<void>(status_.SetText(
                L"ComboBox selection changed — still a native notification."));
        } else if (event.IsClicked(sky_) || event.IsClicked(cosmos_)) {
            static_cast<void>(status_.SetText(L"RadioButton choice changed."));
        } else if (event.Is(items_, LBN_SELCHANGE)) {
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
