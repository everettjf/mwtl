#include <mwtl/mwtl.h>

#include <type_traits>

using mwtl::operator""_dip;

static_assert(!std::is_copy_constructible_v<mwtl::NativeControl>);
static_assert(std::is_copy_constructible_v<mwtl::WindowWakeup>);

class ProbeWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        mwtl::ControlHost ui{*this};
        ui.Add(text_, L"Public API probe");
        ui.Add(button_, L"Close");
        SetLayout(mwtl::Column().Margin(12.0_dip).Gap(8.0_dip)
            .Add(text_, mwtl::Stretch())
            .Add(button_, mwtl::Fixed(34.0_dip)));
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (!event.IsClicked(button_)) return mwtl::EventResult::Propagate();
        Close();
        return mwtl::EventResult::Handled();
    }

private:
    mwtl::Label text_;
    mwtl::Button button_;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    return mwtl::RunApplication<ProbeWindow>(instance, show);
}

