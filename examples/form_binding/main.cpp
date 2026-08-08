#include <mwtl/mwtl.h>

#include <optional>
#include <string>

using mwtl::operator""_dip;

namespace {

struct ProfileSettings {
    std::wstring display_name = L"Ada Lovelace";
    bool notifications = true;
    int workspace = 1;
};

class FormBindingWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        SetTitle(L"Profile settings - live binding and validation");

        mwtl::ControlHost ui{*this};
        ui.Add(title_, L"Profile settings");
        ui.Add(subtitle_, L"Edit native controls, validate once, and keep the model explicit.");
        ui.Add(form_group_, L"Account");
        ui.Add(name_label_, L"Display name");
        ui.Add(name_, L"");
        ui.Add(workspace_label_, L"Default workspace");
        ui.Add(workspace_);
        ui.Add(notifications_, L"Show activity notifications");
        ui.Add(validation_, L"");
        ui.Add(preview_group_, L"Live model preview");
        ui.Add(preview_title_, L"Ada Lovelace");
        ui.Add(preview_detail_, L"Design workspace  |  Notifications on");
        ui.Add(preview_hint_, L"The preview updates only after a valid Pull().");
        ui.Add(reset_, L"Reset");
        ui.Add(apply_, L"Apply settings");
        ui.Add(status_, L"Ready");

        mwtl::Must(mwtl::AddItems(workspace_, {
            L"Personal workspace", L"Design workspace", L"Engineering workspace"}),
            "populate workspaces");

        name_binding_.emplace(
            static_cast<mwtl::NativeControl&>(name_), model_.display_name,
            [](const mwtl::NativeControl& control) { return control.GetText(); },
            [](mwtl::NativeControl& control, const std::wstring& value) {
                return control.SetText(value);
            },
            [](const std::wstring& value) {
                if (value.size() < 2) {
                    return mwtl::ValidationResult::Reject(
                        L"Enter at least two characters.");
                }
                if (value.size() > 32) {
                    return mwtl::ValidationResult::Reject(
                        L"Keep the display name under 33 characters.");
                }
                return mwtl::ValidationResult::Accept();
            });
        notifications_binding_.emplace(notifications_, model_.notifications,
            [](const mwtl::CheckBox& control) { return control.IsChecked(); },
            [](mwtl::CheckBox& control, const bool& value) {
                control.SetChecked(value);
                return true;
            });
        workspace_binding_.emplace(workspace_, model_.workspace,
            [](const mwtl::ComboBox& control) { return control.GetSelection(); },
            [](mwtl::ComboBox& control, const int& value) {
                return control.SetSelection(value);
            });

        PushModel();
        ApplyFont(GetDpiContext().GetDpi());
        mwtl::SetAccessibleName(validation_.GetHwnd(), L"Validation message");
        mwtl::SetDialogDefaultButton(
            GetHwnd(), static_cast<UINT>(apply_.GetId().value));

        SetLayout(
            mwtl::Column()
                .Margin(28.0_dip)
                .Gap(12.0_dip)
                .Add(title_, mwtl::Fixed(38.0_dip))
                .Add(subtitle_, mwtl::Fixed(28.0_dip))
                .Add(
                    mwtl::Row()
                        .Gap(22.0_dip)
                        .Add(BuildFormPanel(), mwtl::Fixed(510.0_dip))
                        .Add(BuildPreviewPanel(), mwtl::Stretch()),
                    mwtl::Stretch(1.0f, 280.0_dip))
                .Add(
                    mwtl::Row()
                        .Gap(10.0_dip)
                        .Add(status_, mwtl::Stretch())
                        .Add(reset_, mwtl::Fixed(110.0_dip))
                        .Add(apply_, mwtl::Fixed(150.0_dip)),
                    mwtl::Fixed(38.0_dip)));

        name_.SelectAll();
        name_.Focus();
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (event.IsClicked(apply_)) {
            PullModel(true);
            return mwtl::EventResult::Handled();
        }
        if (event.IsClicked(reset_)) {
            model_ = {};
            PushModel();
            validation_.SetText(L"");
            status_.SetText(L"Defaults restored with three Push() calls.");
            return mwtl::EventResult::Handled();
        }
        if (event.Is(name_, EN_CHANGE) ||
            event.Is(workspace_, CBN_SELCHANGE) ||
            event.IsClicked(notifications_)) {
            PullModel(false);
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

    mwtl::EventResult OnDpiChanged(
        const mwtl::DpiChangedEvent& event) override {
        ApplyFont(event.dpi_x);
        return mwtl::EventResult::Propagate();
    }

private:
    using TextBinding = mwtl::ValueBinding<mwtl::NativeControl, std::wstring>;
    using CheckBinding = mwtl::ValueBinding<mwtl::CheckBox, bool>;
    using SelectionBinding = mwtl::ValueBinding<mwtl::ComboBox, int>;

    mwtl::LayoutNode BuildFormPanel() {
        return mwtl::Overlay()
            .Add(form_group_)
            .Add(
                mwtl::Column()
                    .Margin({22.0_dip, 34.0_dip, 22.0_dip, 18.0_dip})
                    .Gap(8.0_dip)
                    .Add(name_label_, mwtl::Fixed(24.0_dip))
                    .Add(name_, mwtl::Fixed(34.0_dip))
                    .Add(validation_, mwtl::Fixed(28.0_dip))
                    .Add(workspace_label_, mwtl::Fixed(24.0_dip))
                    .Add(workspace_, mwtl::Fixed(34.0_dip), {
                        .native_size = mwtl::SizeDip{0.0_dip, 160.0_dip},
                    })
                    .Add(notifications_, mwtl::Fixed(30.0_dip)));
    }

    mwtl::LayoutNode BuildPreviewPanel() {
        return mwtl::Overlay()
            .Add(preview_group_)
            .Add(
                mwtl::Column()
                    .Margin({24.0_dip, 40.0_dip, 24.0_dip, 22.0_dip})
                    .Gap(12.0_dip)
                    .Add(preview_title_, mwtl::Fixed(40.0_dip))
                    .Add(preview_detail_, mwtl::Fixed(34.0_dip))
                    .Add(preview_hint_, mwtl::Stretch()));
    }

    bool PullModel(bool committed) {
        const auto name_result = name_binding_->Pull();
        if (!name_result.accepted) {
            validation_.SetText(L"Check: " + name_result.message);
            status_.SetText(L"Model unchanged: validation rejected the candidate.");
            if (committed) {
                name_.Focus();
                name_.SelectAll();
            }
            return false;
        }
        validation_.SetText(L"Valid - looks good");
        workspace_binding_->Pull();
        notifications_binding_->Pull();
        UpdatePreview();
        status_.SetText(committed
            ? L"Settings applied to the model."
            : L"Valid change pulled into the model.");
        return true;
    }

    void PushModel() {
        const bool pushed = name_binding_->Push() && workspace_binding_->Push() &&
            notifications_binding_->Push();
        if (!pushed) status_.SetText(L"A control rejected a model value.");
        UpdatePreview();
    }

    void UpdatePreview() {
        static constexpr const wchar_t* workspaces[]{
            L"Personal workspace", L"Design workspace", L"Engineering workspace"};
        const int safe_index = (model_.workspace >= 0 && model_.workspace < 3)
            ? model_.workspace : 0;
        preview_title_.SetText(model_.display_name);
        preview_detail_.SetText(std::wstring(workspaces[safe_index]) +
            (model_.notifications ? L"  |  Notifications on" : L"  |  Quiet mode"));
    }

    void ApplyFont(UINT dpi) {
        if (!font_.CreateMessageFont(dpi)) return;
        for (HWND control : {
                 title_.GetHwnd(), subtitle_.GetHwnd(), form_group_.GetHwnd(),
                 name_label_.GetHwnd(), name_.GetHwnd(), workspace_label_.GetHwnd(),
                 workspace_.GetHwnd(), notifications_.GetHwnd(), validation_.GetHwnd(),
                 preview_group_.GetHwnd(), preview_title_.GetHwnd(),
                 preview_detail_.GetHwnd(), preview_hint_.GetHwnd(), reset_.GetHwnd(),
                 apply_.GetHwnd(), status_.GetHwnd()}) {
            mwtl::SetControlFont(control, font_.GetHandle());
        }
    }

    ProfileSettings model_;
    std::optional<TextBinding> name_binding_;
    std::optional<CheckBinding> notifications_binding_;
    std::optional<SelectionBinding> workspace_binding_;
    mwtl::Label title_, subtitle_, name_label_, workspace_label_, validation_;
    mwtl::Label preview_title_, preview_detail_, preview_hint_, status_;
    mwtl::GroupBox form_group_, preview_group_;
    mwtl::TextBox name_;
    mwtl::ComboBox workspace_;
    mwtl::CheckBox notifications_;
    mwtl::Button reset_, apply_;
    mwtl::UiFont font_;
};

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    return mwtl::RunApplication<FormBindingWindow>(instance, show, {
        .title = L"Profile settings",
        .initial_bounds = {{0.0_dip, 0.0_dip}, {920.0_dip, 560.0_dip}},
        .use_default_bounds = false,
        .appearance = {.color_mode = mwtl::ColorMode::system,
                       .backdrop = mwtl::Backdrop::mica},
    });
}
