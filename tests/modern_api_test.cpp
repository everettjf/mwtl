#include <mwtl/mwtl.h>

#include <array>
#include <chrono>
#include <cstdlib>
#include <stdexcept>

using namespace std::chrono_literals;
using mwtl::operator""_dip;

namespace {

bool command_seen = false;
bool key_seen = false;
bool custom_seen = false;
bool timer_seen = false;
bool notify_seen = false;
int notify_failure = 0;

class ModernApiWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        mwtl::ControlHost ui{*this};
        ui.Add(label_, kLabel, L"Modern API test", {{8.0_dip, 8.0_dip}, {180.0_dip, 24.0_dip}});
        ui.Add(text_, kText, L"native edit", {{8.0_dip, 40.0_dip}, {180.0_dip, 28.0_dip}});
        ui.Add(button_, kButton, L"Verify", {{8.0_dip, 76.0_dip}, {100.0_dip, 28.0_dip}});
        ui.Add(check_, kCheck, L"Checked", {{120.0_dip, 76.0_dip}, {100.0_dip, 28.0_dip}});
        ui.Add(combo_, kCombo, {{8.0_dip, 112.0_dip}, {180.0_dip, 120.0_dip}});
        ui.Add(progress_, kProgress, {{8.0_dip, 148.0_dip}, {180.0_dip, 20.0_dip}});
        ui.Add(group_, kGroup, L"Choices", {{208.0_dip, 8.0_dip}, {180.0_dip, 92.0_dip}});
        ui.Add(radio_, kRadio, L"Selected", {{224.0_dip, 36.0_dip}, {120.0_dip, 24.0_dip}});
        ui.Add(list_, kList, {{208.0_dip, 108.0_dip}, {180.0_dip, 72.0_dip}});
        ui.Add(slider_, kSlider, {{8.0_dip, 180.0_dip}, {180.0_dip, 28.0_dip}});

        const auto require = [](bool created, const char* name) {
            if (!created) throw std::runtime_error(name);
        };
        require(tree_.Create(*this, {220}, {{400.0_dip, 8.0_dip}, {120.0_dip, 80.0_dip}}), "TreeView creation failed");
        require(list_view_.Create(*this, {221}, {{400.0_dip, 92.0_dip}, {160.0_dip, 80.0_dip}}), "ListView creation failed");
        require(header_.Create(*this, {222}, {{400.0_dip, 176.0_dip}, {160.0_dip, 24.0_dip}}), "Header creation failed");
        require(tabs_.Create(*this, {223}, {{400.0_dip, 204.0_dip}, {160.0_dip, 48.0_dip}}), "TabControl creation failed");
        require(combo_ex_.Create(*this, {224}, {{570.0_dip, 8.0_dip}, {140.0_dip, 100.0_dip}}), "ComboBoxEx creation failed");
        require(date_.Create(*this, {225}, {{570.0_dip, 44.0_dip}, {140.0_dip, 28.0_dip}}), "DateTimePicker creation failed");
        require(calendar_.Create(*this, {226}, {{720.0_dip, 8.0_dip}, {180.0_dip, 150.0_dip}}), "MonthCalendar creation failed");
        require(hot_key_.Create(*this, {227}, {{570.0_dip, 76.0_dip}, {140.0_dip, 28.0_dip}}), "HotKey creation failed");
        require(ip_.Create(*this, {228}, {{570.0_dip, 108.0_dip}, {140.0_dip, 28.0_dip}}), "IpAddress creation failed");
        require(spin_text_.Create(*this, {238}, L"0", {{530.0_dip, 140.0_dip}, {40.0_dip, 28.0_dip}}), "UpDown buddy creation failed");
        require(spin_.Create(*this, {229}, {{570.0_dip, 140.0_dip}, {28.0_dip, 28.0_dip}}), "UpDown creation failed");
        require(link_.Create(*this, {230}, L"<a href=\"https://example.test\">link</a>", {{570.0_dip, 172.0_dip}, {140.0_dip, 28.0_dip}}), "SysLink creation failed");
        require(rebar_.Create(*this, {231}, {{8.0_dip, 220.0_dip}, {240.0_dip, 36.0_dip}}), "Rebar creation failed");
        require(toolbar_.Create(rebar_, {232}, {{0.0_dip, 0.0_dip}, {180.0_dip, 28.0_dip}}), "Toolbar creation failed");
        require(pager_.Create(*this, {233}, {{8.0_dip, 260.0_dip}, {240.0_dip, 36.0_dip}}), "Pager creation failed");
        require(pager_label_.Create(pager_, {234}, L"pager", {{0.0_dip, 0.0_dip}, {300.0_dip, 24.0_dip}}), "Pager child creation failed");
        require(animation_.Create(*this, {235}, {{260.0_dip, 260.0_dip}, {80.0_dip, 36.0_dip}}), "Animation creation failed");
        require(scroll_.Create(*this, {236}, {{350.0_dip, 260.0_dip}, {160.0_dip, 24.0_dip}}), "ScrollBar creation failed");
        require(status_bar_.Create(*this, {237}, {{0.0_dip, 300.0_dip}, {600.0_dip, 24.0_dip}}), "StatusBar creation failed");
        require(tooltip_.Create(rebar_.GetHwnd()), "Tooltip creation failed");
        require(images_.Create(16, 16), "ImageList creation failed");
        require(timer_.Start(*this, kTimer, 1ms), "Timer creation failed");

        check_.SetChecked(true);
        static_cast<void>(combo_.AddItem(L"first"));
        static_cast<void>(combo_.AddItem(L"second"));
        progress_.SetRange(0, 100);
        progress_.SetValue(64);
        radio_.SetChecked(true);
        static_cast<void>(list_.AddItem(L"first"));
        static_cast<void>(list_.AddItem(L"second"));
        slider_.SetRange(0, 100);
        slider_.SetValue(73);
        const HTREEITEM root = tree_.AddItem(L"root");
        static_cast<void>(tree_.AddItem(L"child", root));
        static_cast<void>(tree_.Expand(root));
        static_cast<void>(list_view_.AddColumn(L"name", 80));
        const int list_row = list_view_.AddItem(L"item");
        static_cast<void>(list_view_.SetSubItem(list_row, 0, L"updated"));
        static_cast<void>(header_.AddColumn(L"header", 80));
        static_cast<void>(tabs_.AddTab(L"tab"));
        static_cast<void>(combo_ex_.AddItem(L"combo"));
        static_cast<void>(combo_ex_.SetSelection(0));
        hot_key_.SetValue('K', HOTKEYF_CONTROL);
        ip_.SetValue(127, 0, 0, 1);
        spin_.SetBuddy(spin_text_); spin_.SetRange(0, 100); spin_.SetValue(42);
        static_cast<void>(toolbar_.AddTextButton({600}, L"Tool")); toolbar_.AutoSize();
        static_cast<void>(rebar_.AddBand(toolbar_, L"Band", 120));
        pager_.SetChild(pager_label_);
        scroll_.SetRange(0, 100); scroll_.SetValue(31);
        static_cast<void>(mwtl::InitializeFlatScrollBars(scroll_.GetHwnd()));
        const std::array status_parts{120, -1};
        static_cast<void>(status_bar_.SetParts(status_parts));
        static_cast<void>(status_bar_.SetPartText(0, L"ready"));
        static_cast<void>(tooltip_.AddTool(toolbar_.GetHwnd(), L"tooltip"));
        if (!combo_.SetSelection(1) || combo_.GetSelection() != 1 ||
            !check_.IsChecked() || progress_.GetValue() != 64 ||
            !radio_.IsChecked() || !list_.SetSelection(1) ||
            list_.GetSelection() != 1 || slider_.GetValue() != 73) {
            throw std::runtime_error("modern controls state verification failed");
        }

        button_.Click();
        ::SendMessageW(GetHwnd(), WM_KEYDOWN, VK_SPACE, 1);
        ::SendMessageW(GetHwnd(), kCustomMessage, 42, 0);
        NMHDR notification{button_.GetHwnd(), static_cast<UINT_PTR>(kButton.value), NM_CLICK};
        ::SendMessageW(GetHwnd(), WM_NOTIFY, notification.idFrom,
                       reinterpret_cast<LPARAM>(&notification));
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (event.IsClicked(button_)) {
            command_seen = text_.GetText() == L"native edit";
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

    mwtl::EventResult OnKeyDown(const mwtl::KeyEvent& event) override {
        key_seen = event.virtual_key == VK_SPACE && event.repeat_count == 1;
        return mwtl::EventResult::Handled();
    }

    mwtl::EventResult OnNotify(const mwtl::NotifyEvent& event) override {
        if (event.GetCode() != NM_CLICK) return mwtl::EventResult::Propagate();
        notify_seen = true;
        if (event.GetId() != kButton) notify_failure |= 1;
        if (event.GetControl() != button_.GetHwnd()) notify_failure |= 4;
        return mwtl::EventResult::Handled(1);
    }

    mwtl::EventResult OnMessage(const mwtl::WindowMessage& message) override {
        if (message.id == kCustomMessage) {
            custom_seen = message.wparam == 42;
            return mwtl::EventResult::Handled(77);
        }
        return mwtl::EventResult::Propagate();
    }

    mwtl::EventResult OnTimer(mwtl::TimerId id) override {
        if (id == kTimer) {
            timer_seen = true;
            timer_.Stop();
            ::PostMessageW(GetHwnd(), WM_CLOSE, 0, 0);
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

private:
    static constexpr UINT kCustomMessage = WM_APP + 76;
    static constexpr mwtl::ControlId kLabel{200};
    static constexpr mwtl::ControlId kText{201};
    static constexpr mwtl::ControlId kButton{202};
    static constexpr mwtl::ControlId kCheck{203};
    static constexpr mwtl::ControlId kCombo{204};
    static constexpr mwtl::ControlId kProgress{205};
    static constexpr mwtl::ControlId kGroup{206};
    static constexpr mwtl::ControlId kRadio{207};
    static constexpr mwtl::ControlId kList{208};
    static constexpr mwtl::ControlId kSlider{209};
    static constexpr mwtl::TimerId kTimer{9};

    mwtl::Label label_;
    mwtl::TextBox text_;
    mwtl::Button button_;
    mwtl::CheckBox check_;
    mwtl::ComboBox combo_;
    mwtl::ProgressBar progress_;
    mwtl::GroupBox group_;
    mwtl::RadioButton radio_;
    mwtl::ListBox list_;
    mwtl::Slider slider_;
    mwtl::TreeView tree_; mwtl::ListView list_view_; mwtl::Header header_;
    mwtl::TabControl tabs_; mwtl::ComboBoxEx combo_ex_; mwtl::DateTimePicker date_;
    mwtl::MonthCalendar calendar_; mwtl::HotKey hot_key_; mwtl::IpAddress ip_;
    mwtl::TextBox spin_text_; mwtl::UpDown spin_; mwtl::SysLink link_; mwtl::Rebar rebar_; mwtl::Toolbar toolbar_;
    mwtl::Pager pager_; mwtl::Label pager_label_; mwtl::Animation animation_;
    mwtl::ScrollBar scroll_; mwtl::StatusBar status_bar_; mwtl::Tooltip tooltip_;
    mwtl::ImageList images_;
    mwtl::UiTimer timer_;
};

}  // namespace

int main() {
    const int result =
        mwtl::Application(::GetModuleHandleW(nullptr)).Run<ModernApiWindow>(SW_HIDE);
    if (result != 0) return 10;
    if (!command_seen) return 11;
    if (!key_seen) return 12;
    if (!custom_seen) return 13;
    if (!timer_seen) return 14;
    if (!notify_seen) return 15;
    if (notify_failure != 0) return 20 + notify_failure;
    return EXIT_SUCCESS;
}
