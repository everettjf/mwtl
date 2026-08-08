#include <mwtl/mwtl.h>

#include <string>

using mwtl::operator""_dip;

namespace {

class CommandsWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        SetTitle(L"Command studio - one action model, three surfaces");
        BuildCommands();

        mwtl::ControlHost ui{*this};
        ui.Add(title_, L"Command studio");
        ui.Add(subtitle_, L"Menu, toolbar, and keyboard shortcuts share the same command state.");
        ui.Add(toolbar_);
        mwtl::TextBoxOptions editor_options;
        editor_options.style |= ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL;
        ui.Add(editor_, L"Write something here, then save it.\r\n\r\nCtrl+N creates a fresh note. Ctrl+S saves. F11 toggles focus mode.", editor_options);
        ui.Add(inspector_, L"Command state");
        ui.Add(save_state_, L"Save is disabled until the document changes.");
        ui.Add(focus_state_, L"Focus mode: off");
        ui.Add(hint_, L"Try the toolbar, the Command menu, or the shortcuts - every route dispatches through CommandSet.");
        ui.Add(status_, L"Ready");

        for (const auto& command : commands_.GetCommands()) {
            if (command.GetId() != kExit) mwtl::Must(toolbar_.AddCommand(command), "add toolbar command");
        }
        toolbar_.AutoSize();
        BuildMenu();
        mwtl::Must(accelerators_.Create(commands_), "create accelerator table");
        SetAccelerators(accelerators_.GetHandle());

        ApplyFont(GetDpiContext().GetDpi());
        mwtl::ApplyWindowAppearance(GetHwnd(), {mwtl::ColorMode::system, mwtl::Backdrop::mica});
        SetLayout(mwtl::Column().Margin(24.0_dip).Gap(10.0_dip)
            .Add(title_, mwtl::Fixed(34.0_dip))
            .Add(subtitle_, mwtl::Fixed(24.0_dip))
            .Add(toolbar_, mwtl::Fixed(38.0_dip))
            .Add(mwtl::Row().Gap(14.0_dip)
                .Add(editor_, mwtl::Stretch())
                .Add(mwtl::Overlay()
                    .Add(inspector_)
                    .Add(mwtl::Column().Margin({20.0_dip, 34.0_dip, 20.0_dip, 16.0_dip}).Gap(10.0_dip)
                        .Add(save_state_, mwtl::Auto())
                        .Add(focus_state_, mwtl::Auto())
                        .Add(hint_, mwtl::Stretch())), mwtl::Fixed(310.0_dip)),
                mwtl::Stretch())
            .Add(status_, mwtl::Fixed(28.0_dip)));
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (event.Is(editor_, EN_CHANGE) && !updating_) {
            dirty_ = true;
            SyncState(L"Document changed");
            return mwtl::EventResult::Handled();
        }
        return commands_.Dispatch(event);
    }

    mwtl::EventResult OnDpiChanged(const mwtl::DpiChangedEvent& event) override { ApplyFont(event.dpi_x); return mwtl::EventResult::Propagate(); }

private:
    static constexpr mwtl::ControlId kNew{600};
    static constexpr mwtl::ControlId kSave{601};
    static constexpr mwtl::ControlId kFocus{602};
    static constexpr mwtl::ControlId kExit{603};

    void BuildCommands() {
        commands_
            .Add(mwtl::Command(kNew, L"New note", [this] {
                updating_ = true;
                editor_.SetText(L"Untitled note\r\n\r\n");
                updating_ = false;
                dirty_ = false;
                editor_.Focus();
                SyncState(L"Created a fresh note");
            }).SetShortcut({FVIRTKEY | FCONTROL, 'N'}))
            .Add(mwtl::Command(kSave, L"Save", [this] {
                dirty_ = false;
                SyncState(L"Saved locally (demo)");
            }).SetEnabled(false).SetShortcut({FVIRTKEY | FCONTROL, 'S'}))
            .Add(mwtl::Command(kFocus, L"Focus mode", [this] {
                focus_mode_ = !focus_mode_;
                SyncState(focus_mode_ ? L"Focus mode enabled" : L"Focus mode disabled");
                editor_.Focus();
            }).SetShortcut({FVIRTKEY, VK_F11}))
            .Add(mwtl::Command(kExit, L"Exit", [this] { static_cast<void>(Close()); }));
    }

    void BuildMenu() {
        mwtl::Menu bar;
        mwtl::Menu popup;
        mwtl::Must(bar.Create(), "create menu bar");
        mwtl::Must(popup.CreatePopup(), "create command menu");
        menu_handle_ = popup.GetHandle();
        mwtl::Must(popup.AppendCommand(*commands_.Find(kNew)), "append new command");
        mwtl::Must(popup.AppendCommand(*commands_.Find(kSave)), "append save command");
        mwtl::Must(popup.AppendSeparator(), "append separator");
        mwtl::Must(popup.AppendCommand(*commands_.Find(kFocus)), "append focus command");
        mwtl::Must(popup.AppendSeparator(), "append separator");
        mwtl::Must(popup.AppendCommand(*commands_.Find(kExit)), "append exit command");
        mwtl::Must(bar.AppendSubmenu(std::move(popup), L"Command"), "append submenu");
        mwtl::Must(bar.AttachToWindow(GetHwnd()), "attach menu");
    }

    void SyncState(std::wstring_view message) {
        auto* save = commands_.Find(kSave);
        auto* focus = commands_.Find(kFocus);
        save->SetEnabled(dirty_);
        focus->SetChecked(focus_mode_);
        toolbar_.UpdateCommand(*save);
        toolbar_.UpdateCommand(*focus);
        if (menu_handle_) {
            ::EnableMenuItem(menu_handle_, kSave.value, MF_BYCOMMAND | (dirty_ ? MF_ENABLED : MF_GRAYED));
            ::CheckMenuItem(menu_handle_, kFocus.value, MF_BYCOMMAND | (focus_mode_ ? MF_CHECKED : MF_UNCHECKED));
            ::DrawMenuBar(GetHwnd());
        }
        save_state_.SetText(dirty_ ? L"Save is enabled: the document has changes." : L"Save is disabled: everything is up to date.");
        focus_state_.SetText(focus_mode_ ? L"Focus mode: on (checked everywhere)" : L"Focus mode: off");
        status_.SetText(message);
    }

    void ApplyFont(UINT dpi) {
        if (!font_.CreateMessageFont(dpi)) return;
        for (HWND child = ::GetWindow(GetHwnd(), GW_CHILD); child; child = ::GetWindow(child, GW_HWNDNEXT))
            ::SendMessageW(child, WM_SETFONT, reinterpret_cast<WPARAM>(font_.GetHandle()), TRUE);
    }

    mwtl::CommandSet commands_;
    mwtl::AcceleratorTable accelerators_;
    HMENU menu_handle_{};
    bool dirty_{};
    bool focus_mode_{};
    bool updating_{};
    mwtl::Label title_, subtitle_, save_state_, focus_state_, hint_, status_;
    mwtl::Toolbar toolbar_;
    mwtl::TextBox editor_;
    mwtl::GroupBox inspector_;
    mwtl::UiFont font_;
};

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    return mwtl::RunApplication<CommandsWindow>(instance, show, {.title = L"Command studio", .initial_bounds = {{}, {1080.0_dip, 680.0_dip}}, .use_default_bounds = false});
}
