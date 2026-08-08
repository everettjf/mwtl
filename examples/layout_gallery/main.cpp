#include <mwtl/mwtl.h>

#include <string>

using mwtl::operator""_dip;

namespace {

class LayoutGalleryWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        SetTitle(L"Responsive layout gallery");
        mwtl::ControlHost ui{*this};
        ui.Add(title_, L"Responsive layout gallery");
        ui.Add(subtitle_, L"Resize the window and switch density to see nested Row, Column, Overlay, Auto, Fixed, and Stretch behavior.");
        ui.Add(compact_, L"Compact");
        ui.Add(comfortable_, L"Comfortable");
        ui.Add(long_text_, L"Toggle long content");
        ui.Add(profile_, L"Profile card - Overlay");
        ui.Add(avatar_, L"AL");
        ui.Add(name_, L"Ada Lovelace");
        ui.Add(role_, L"Computing pioneer");
        ui.Add(active_, L"Active");
        ui.Add(metrics_, L"Weighted stretch row");
        ui.Add(metric_a_, L"12\r\nProjects");
        ui.Add(metric_b_, L"48\r\nReviews");
        ui.Add(metric_c_, L"99%\r\nUptime");
        ui.Add(form_, L"Auto labels + stretch inputs");
        ui.Add(email_label_, L"Email");
        ui.Add(email_, L"ada@example.com");
        ui.Add(team_label_, L"Team");
        ui.Add(team_);
        ui.Add(save_, L"Save profile");
        ui.Add(explainer_, L"Layout recipe");
        ui.Add(recipe_, L"Root Column\r\n  Toolbar: Auto\r\n  Content Row: Stretch\r\n    Cards: weighted Stretch\r\n  Footer: Fixed");
        ui.Add(status_, L"Comfortable density | resize to test");
        mwtl::Must(mwtl::AddItems(team_, {L"Research", L"Design systems", L"Developer experience"}), "populate teams");
        team_.SetSelection(2);
        comfortable_.SetChecked(true);
        mwtl::SetDialogDefaultButton(GetHwnd(), static_cast<UINT>(save_.GetId().value));
        mwtl::ApplyWindowAppearance(GetHwnd(), {mwtl::ColorMode::system, mwtl::Backdrop::mica});
        ApplyFont(GetDpiContext().GetDpi());
        RebuildLayout();
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (event.IsClicked(compact_)) {
            compact_.SetChecked(true); comfortable_.SetChecked(false); compact_mode_ = true;
            status_.SetText(L"Compact density | gaps and card padding reduced");
            RebuildLayout();
            return mwtl::EventResult::Handled();
        }
        if (event.IsClicked(comfortable_)) {
            compact_.SetChecked(false); comfortable_.SetChecked(true); compact_mode_ = false;
            status_.SetText(L"Comfortable density | resize to test");
            RebuildLayout();
            return mwtl::EventResult::Handled();
        }
        if (event.IsClicked(long_text_)) {
            long_content_ = !long_content_;
            role_.SetText(long_content_ ? L"Mathematician, writer, and visionary of general-purpose computing" : L"Computing pioneer");
            email_label_.SetText(long_content_ ? L"Primary contact email address" : L"Email");
            status_.SetText(long_content_ ? L"Long content enabled | Auto measurement updated" : L"Short content restored");
            RebuildLayout();
            return mwtl::EventResult::Handled();
        }
        if (event.IsClicked(save_)) {
            status_.SetText(L"Saved | the footer remains fixed while content stretches");
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

    mwtl::EventResult OnDpiChanged(const mwtl::DpiChangedEvent& event) override {
        ApplyFont(event.dpi_x); return mwtl::EventResult::Propagate();
    }

private:
    mwtl::LayoutNode ProfileCard(mwtl::Dip pad, mwtl::Dip gap) {
        return mwtl::Overlay().Add(profile_).Add(
            mwtl::Column().Margin({pad, 36.0_dip, pad, pad}).Gap(gap)
                .Add(mwtl::Row().Gap(gap)
                    .Add(avatar_, mwtl::Fixed(62.0_dip), {.alignment = mwtl::CrossAlignment::center})
                    .Add(mwtl::Column().Gap(4.0_dip).Add(name_, mwtl::Auto()).Add(role_, mwtl::Stretch()), mwtl::Stretch())
                    .Add(active_, mwtl::Fixed(74.0_dip), {.alignment = mwtl::CrossAlignment::start}), mwtl::Fixed(110.0_dip))
                .Add(mwtl::Column(), mwtl::Stretch())
                .Add(mwtl::Overlay().Add(metrics_).Add(
                    mwtl::Row().Margin({14.0_dip, 30.0_dip, 14.0_dip, 10.0_dip}).Gap(gap)
                        .Add(metric_a_, mwtl::Stretch(1.0f)).Add(metric_b_, mwtl::Stretch(1.4f)).Add(metric_c_, mwtl::Stretch(1.0f))),
                    mwtl::Fixed(96.0_dip)));
    }

    mwtl::LayoutNode FormCard(mwtl::Dip pad, mwtl::Dip gap) {
        const auto label_width = long_content_ ? 190.0_dip : 74.0_dip;
        return mwtl::Overlay().Add(form_).Add(
            mwtl::Column().Margin({pad, 36.0_dip, pad, pad}).Gap(gap)
                .Add(mwtl::Row().Gap(gap).Add(email_label_, mwtl::Fixed(label_width), {.alignment = mwtl::CrossAlignment::center}).Add(email_, mwtl::Stretch()), mwtl::Fixed(38.0_dip))
                .Add(mwtl::Row().Gap(gap).Add(team_label_, mwtl::Fixed(label_width), {.alignment = mwtl::CrossAlignment::center}).Add(team_, mwtl::Stretch()), mwtl::Fixed(38.0_dip))
                .Add(mwtl::Row().Add(mwtl::Column(), mwtl::Stretch()).Add(save_, mwtl::Fixed(140.0_dip)), mwtl::Fixed(42.0_dip))
                .Add(mwtl::Overlay().Add(explainer_).Add(mwtl::Column().Margin({14.0_dip, 30.0_dip, 14.0_dip, 12.0_dip}).Add(recipe_, mwtl::Stretch())), mwtl::Stretch()));
    }

    void RebuildLayout() {
        const auto gap = compact_mode_ ? 7.0_dip : 12.0_dip;
        const auto pad = compact_mode_ ? 14.0_dip : 22.0_dip;
        SetLayout(mwtl::Column().Margin(compact_mode_ ? 16.0_dip : 24.0_dip).Gap(gap)
            .Add(title_, mwtl::Fixed(34.0_dip)).Add(subtitle_, mwtl::Fixed(28.0_dip))
            .Add(mwtl::Row().Gap(8.0_dip).Add(compact_, mwtl::Fixed(105.0_dip)).Add(comfortable_, mwtl::Fixed(120.0_dip)).Add(long_text_, mwtl::Fixed(160.0_dip)), mwtl::Fixed(34.0_dip))
            .Add(mwtl::Row().Gap(gap).Add(ProfileCard(pad, gap), mwtl::Fixed(560.0_dip)).Add(FormCard(pad, gap), mwtl::Stretch()), mwtl::Stretch())
            .Add(status_, mwtl::Fixed(30.0_dip)));
    }
    void ApplyFont(UINT dpi) {
        if (!font_.CreateMessageFont(dpi)) return;
        for (HWND child = ::GetWindow(GetHwnd(), GW_CHILD); child; child = ::GetWindow(child, GW_HWNDNEXT))
            ::SendMessageW(child, WM_SETFONT, reinterpret_cast<WPARAM>(font_.GetHandle()), TRUE);
    }
    bool compact_mode_{};
    bool long_content_{};
    mwtl::Label title_, subtitle_, avatar_, name_, role_, active_, metric_a_, metric_b_, metric_c_, email_label_, team_label_, recipe_, status_;
    mwtl::RadioButton compact_, comfortable_;
    mwtl::Button long_text_, save_;
    mwtl::GroupBox profile_, metrics_, form_, explainer_;
    mwtl::TextBox email_;
    mwtl::ComboBox team_;
    mwtl::UiFont font_;
};

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    return mwtl::RunApplication<LayoutGalleryWindow>(instance, show, {.title = L"Responsive layout gallery", .initial_bounds = {{}, {1180.0_dip, 760.0_dip}}, .use_default_bounds = false});
}
