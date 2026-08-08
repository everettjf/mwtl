#include <mwtl/mwtl.h>

#include <array>
#include <string>

using mwtl::operator""_dip;

namespace {

class AppearanceWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        SetTitle(L"Appearance lab");
        mwtl::ControlHost ui{*this};
        ui.Add(title_, L"Appearance lab");
        ui.Add(subtitle_, L"Explore Windows 11 title-bar color, backdrop material, corner policy, and accessibility helpers.");
        ui.Add(settings_, L"Window composition");
        ui.Add(mode_label_, L"Color mode");
        ui.Add(mode_);
        ui.Add(backdrop_label_, L"Backdrop material");
        ui.Add(backdrop_);
        ui.Add(rounded_, L"Prefer rounded corners");
        ui.Add(apply_, L"Apply appearance");
        ui.Add(preview_, L"Live preview");
        ui.Add(preview_title_, L"A thin, native layer");
        ui.Add(preview_body_, L"mwtl asks DWM for modern window composition while keeping normal Win32 controls and explicit HWND ownership.");
        ui.Add(system_, L"");
        ui.Add(note_, L"DWM options are best-effort. High Contrast automatically falls back to system-safe defaults.");
        ui.Add(status_, L"System + Mica");

        mwtl::Must(mwtl::AddItems(mode_, {L"Follow system", L"Light title bar", L"Dark title bar"}), "populate color modes");
        mwtl::Must(mwtl::AddItems(backdrop_, {L"None", L"Mica", L"Acrylic", L"Tabbed"}), "populate backdrops");
        mode_.SetSelection(0);
        backdrop_.SetSelection(1);
        rounded_.SetChecked(true);
        UpdateSystemSummary();
        mwtl::SetAccessibleName(mode_.GetHwnd(), L"Window color mode");
        mwtl::SetAccessibleName(backdrop_.GetHwnd(), L"Window backdrop material");
        mwtl::SetDialogDefaultButton(GetHwnd(), static_cast<UINT>(apply_.GetId().value));
        ApplySelection();
        ApplyFont(GetDpiContext().GetDpi());

        SetLayout(mwtl::Column().Margin(26.0_dip).Gap(11.0_dip)
            .Add(title_, mwtl::Fixed(34.0_dip)).Add(subtitle_, mwtl::Fixed(26.0_dip))
            .Add(mwtl::Row().Gap(16.0_dip)
                .Add(mwtl::Column().Gap(9.0_dip).Add(settings_, mwtl::Fixed(28.0_dip))
                    .Add(mode_label_, mwtl::Auto()).Add(mode_, mwtl::Fixed(38.0_dip))
                    .Add(backdrop_label_, mwtl::Auto()).Add(backdrop_, mwtl::Fixed(38.0_dip))
                    .Add(rounded_, mwtl::Auto()).Add(apply_, mwtl::Fixed(42.0_dip))
                    .Add(system_, mwtl::Auto()).Add(note_, mwtl::Stretch()), mwtl::Fixed(360.0_dip))
                .Add(mwtl::Column().Gap(12.0_dip).Add(preview_, mwtl::Fixed(28.0_dip))
                    .Add(preview_title_, mwtl::Auto()).Add(preview_body_, mwtl::Stretch()).Add(status_, mwtl::Fixed(40.0_dip)), mwtl::Stretch()),
                mwtl::Stretch()));
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (event.IsClicked(apply_) || event.Is(mode_, CBN_SELCHANGE) || event.Is(backdrop_, CBN_SELCHANGE) || event.IsClicked(rounded_)) {
            ApplySelection();
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }
    mwtl::EventResult OnDpiChanged(const mwtl::DpiChangedEvent& event) override { ApplyFont(event.dpi_x); return mwtl::EventResult::Propagate(); }

private:
    void ApplySelection() {
        static constexpr std::array modes{mwtl::ColorMode::system, mwtl::ColorMode::light, mwtl::ColorMode::dark};
        static constexpr std::array backdrops{mwtl::Backdrop::none, mwtl::Backdrop::mica, mwtl::Backdrop::acrylic, mwtl::Backdrop::tabbed};
        const int mode = (std::max)(0, mode_.GetSelection());
        const int backdrop = (std::max)(0, backdrop_.GetSelection());
        const mwtl::AppearanceOptions options{modes[static_cast<size_t>(mode)], backdrops[static_cast<size_t>(backdrop)], rounded_.IsChecked()};
        const bool applied = mwtl::ApplyWindowAppearance(GetHwnd(), options);
        static constexpr const wchar_t* mode_names[]{L"System", L"Light", L"Dark"};
        static constexpr const wchar_t* backdrop_names[]{L"None", L"Mica", L"Acrylic", L"Tabbed"};
        status_.SetText(std::wstring(applied ? L"Applied: " : L"Requested: ") + mode_names[mode] + L" + " + backdrop_names[backdrop]);
        UpdateSystemSummary();
    }
    void UpdateSystemSummary() {
        system_.SetText(std::wstring(L"System preference: ") + (mwtl::IsSystemDarkModePreferred() ? L"dark" : L"light") +
            L"\r\nHigh Contrast: " + (mwtl::IsHighContrastEnabled() ? L"on" : L"off"));
    }
    void ApplyFont(UINT dpi) {
        if (!font_.CreateMessageFont(dpi)) return;
        for (HWND child = ::GetWindow(GetHwnd(), GW_CHILD); child; child = ::GetWindow(child, GW_HWNDNEXT))
            ::SendMessageW(child, WM_SETFONT, reinterpret_cast<WPARAM>(font_.GetHandle()), TRUE);
    }
    mwtl::Label title_, subtitle_, mode_label_, backdrop_label_, preview_title_, preview_body_, system_, note_, status_;
    mwtl::GroupBox settings_, preview_;
    mwtl::ComboBox mode_, backdrop_;
    mwtl::CheckBox rounded_;
    mwtl::Button apply_;
    mwtl::UiFont font_;
};

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    return mwtl::RunApplication<AppearanceWindow>(instance, show, {.title = L"Appearance lab", .initial_bounds = {{}, {1040.0_dip, 680.0_dip}}, .use_default_bounds = false});
}
