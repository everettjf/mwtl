#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

using mwtl::operator""_dip;

class MainWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        if (!SetTitle(L"mwtl hello")) {
            throw std::runtime_error("SetWindowTextW failed while building the main window");
        }
        if (!message_.Create(
                *this, kMessage,
                L"Hello from a native Windows UI with modern C++20 ergonomics.",
                {{28.0_dip, 28.0_dip}, {520.0_dip, 34.0_dip}}) ||
            !button_.Create(
                *this, kButton, L"Make it brighter",
                {{28.0_dip, 82.0_dip}, {180.0_dip, 36.0_dip}})) {
            throw std::runtime_error("Could not create hello controls");
        }
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
