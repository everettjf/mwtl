#include <mwtl/mwtl.h>

using mwtl::operator""_dip;

class MainWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        SetTitle(L"mwtl starter");
        mwtl::ControlHost ui{*this};
        ui.Add(message_, L"A native Windows application generated from the mwtl starter.");
        ui.Add(close_, L"Close");
        SetLayout(mwtl::Column().Margin(24.0_dip).Gap(12.0_dip)
            .Add(message_, mwtl::Stretch())
            .Add(close_, mwtl::Fixed(38.0_dip)));
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (event.IsClicked(close_)) {
            Close();
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

private:
    mwtl::Label message_;
    mwtl::Button close_;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    return mwtl::RunApplication<MainWindow>(instance, show);
}

