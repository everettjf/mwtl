#include <mwtl/mwtl.h>

#include <string>

using mwtl::operator""_dip;

class FormWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        mwtl::ControlHost ui{*this};
        ui.Add(name_label_, L"Display name");
        ui.Add(name_, model_);
        ui.Add(validation_, L"");
        ui.Add(save_, L"Save");
        SetLayout(mwtl::Column().Margin(24.0_dip).Gap(8.0_dip)
            .Add(name_label_, mwtl::Auto())
            .Add(name_, mwtl::Fixed(34.0_dip))
            .Add(validation_, mwtl::Stretch())
            .Add(save_, mwtl::Fixed(38.0_dip)));
        name_.Focus();
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (!event.IsClicked(save_)) return mwtl::EventResult::Propagate();
        const std::wstring candidate = name_.GetText();
        if (candidate.size() < 2) {
            validation_.SetText(L"Enter at least two characters.");
            name_.Focus();
            name_.SelectAll();
        } else {
            model_ = candidate;
            validation_.SetText(L"Saved to the model.");
        }
        return mwtl::EventResult::Handled();
    }

private:
    std::wstring model_ = L"Ada";
    mwtl::Label name_label_, validation_;
    mwtl::TextBox name_;
    mwtl::Button save_;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    return mwtl::RunApplication<FormWindow>(instance, show, {.title = L"Profile"});
}
