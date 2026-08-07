#include <mwtl/mwtl.h>

#include <cstdlib>

using mwtl::operator""_dip;

class MainWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        mwtl::Must(SetTitle(L"mwtl hello"), "set window title");
        mwtl::ControlHost ui{*this};
        ui.Add(
            message_, kMessage,
            L"Hello from a native Windows UI with modern C++20 ergonomics.");
        ui.Add(button_, kButton, L"Make it brighter");
        UseLayout(
            mwtl::Column()
                .Margin(28.0_dip)
                .Gap(20.0_dip)
                .Add(message_, mwtl::Fixed(34.0_dip))
                .Add(button_, mwtl::Fixed(36.0_dip), {
                    .alignment = mwtl::CrossAlignment::start,
                    .preferred_size = mwtl::SizeDip{180.0_dip, 36.0_dip},
                }));
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (event.IsClicked(button_)) {
            static_cast<void>(message_.SetText(
                L"A real BUTTON HWND, dispatched without message-map macros."));
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

private:
    static constexpr mwtl::ControlId kMessage{100};
    static constexpr mwtl::ControlId kButton{101};
    mwtl::Label message_;
    mwtl::Button button_;
};

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE,
    PWSTR,
    int show_command) {
    return mwtl::RunApplication<MainWindow>(
        instance,
        show_command,
        {
            .initial_bounds = {{0.0_dip, 0.0_dip}, {960.0_dip, 320.0_dip}},
            .use_default_bounds = false,
        });
}
