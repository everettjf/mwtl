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

        ui.Add(tree_, {220}, {{400.0_dip, 8.0_dip}, {120.0_dip, 80.0_dip}});
        ui.Add(list_view_, {221}, {{400.0_dip, 92.0_dip}, {160.0_dip, 80.0_dip}});
        ui.Add(header_, {222}, {{400.0_dip, 176.0_dip}, {160.0_dip, 24.0_dip}});
        ui.Add(tabs_, {223}, {{400.0_dip, 204.0_dip}, {160.0_dip, 48.0_dip}});
        ui.Add(combo_ex_, {224}, {{570.0_dip, 8.0_dip}, {140.0_dip, 100.0_dip}});
        ui.Add(date_, {225}, {{570.0_dip, 44.0_dip}, {140.0_dip, 28.0_dip}});
        ui.Add(calendar_, {226}, {{720.0_dip, 8.0_dip}, {180.0_dip, 150.0_dip}});
        ui.Add(hot_key_, {227}, {{570.0_dip, 76.0_dip}, {140.0_dip, 28.0_dip}});
        ui.Add(ip_, {228}, {{570.0_dip, 108.0_dip}, {140.0_dip, 28.0_dip}});
        ui.Add(spin_text_, {238}, L"0", {{530.0_dip, 140.0_dip}, {40.0_dip, 28.0_dip}});
        ui.Add(spin_, {229}, {{570.0_dip, 140.0_dip}, {28.0_dip, 28.0_dip}});
        ui.Add(link_, {230}, L"<a href=\"https://example.test\">link</a>", {{570.0_dip, 172.0_dip}, {140.0_dip, 28.0_dip}});
        ui.Add(rebar_, {231}, {{8.0_dip, 220.0_dip}, {240.0_dip, 36.0_dip}});
        mwtl::ControlHost rebar_ui{rebar_};
        rebar_ui.Add(toolbar_, {232}, {{0.0_dip, 0.0_dip}, {180.0_dip, 28.0_dip}});
        ui.Add(pager_, {233}, {{8.0_dip, 260.0_dip}, {240.0_dip, 36.0_dip}});
        mwtl::ControlHost pager_ui{pager_};
        pager_ui.Add(pager_label_, {234}, L"pager", {{0.0_dip, 0.0_dip}, {300.0_dip, 24.0_dip}});
        ui.Add(animation_, {235}, {{260.0_dip, 260.0_dip}, {80.0_dip, 36.0_dip}});
        ui.Add(scroll_, {236}, {{350.0_dip, 260.0_dip}, {160.0_dip, 24.0_dip}});
        ui.Add(status_bar_, {237}, {{0.0_dip, 300.0_dip}, {600.0_dip, 24.0_dip}});
        const auto require = [](bool created, const char* name) {
            if (!created) throw std::runtime_error(name);
        };
        require(tooltip_.Create(rebar_.GetHwnd()), "Tooltip creation failed");
        require(images_.Create(16, 16), "ImageList creation failed");
        require(timer_.Start(*this, kTimer, 1ms), "Timer creation failed");

        check_.SetChecked(true);
        require(static_cast<bool>(mwtl::AddItems(combo_, {L"first", L"second"})),
                "ComboBox population failed");
        progress_.SetRange(0, 100);
        progress_.SetValue(64);
        radio_.SetChecked(true);
        require(static_cast<bool>(mwtl::AddItems(list_, {L"first", L"second"})),
                "ListBox population failed");
        slider_.SetRange(0, 100);
        slider_.SetValue(73);
        const HTREEITEM root = tree_.AddItem(L"root");
        static_cast<void>(tree_.AddItem(L"child", root));
        static_cast<void>(tree_.Expand(root));
        require(static_cast<bool>(mwtl::AddColumns(list_view_, {{L"name", 80}})),
                "ListView column population failed");
        const int list_row = list_view_.AddItem(L"item");
        static_cast<void>(list_view_.SetSubItem(list_row, 0, L"updated"));
        require(static_cast<bool>(mwtl::AddColumns(header_, {{L"header", 80}})),
                "Header column population failed");
        require(static_cast<bool>(mwtl::AddTabs(tabs_, {L"tab"})),
                "Tab population failed");
        require(static_cast<bool>(mwtl::AddItems(combo_ex_, {L"combo"})),
                "ComboBoxEx population failed");
        static_cast<void>(combo_ex_.SetSelection(0));
        hot_key_.SetValue('K', HOTKEYF_CONTROL);
        ip_.SetValue(127, 0, 0, 1);
        spin_.SetBuddy(spin_text_); spin_.SetRange(0, 100); spin_.SetValue(42);
        require(static_cast<bool>(mwtl::AddButtons(toolbar_, {{{600}, L"Tool"}})),
                "Toolbar button population failed");
        toolbar_.AutoSize();
        static_cast<void>(rebar_.AddBand(toolbar_, L"Band", 120));
        pager_.SetChild(pager_label_);
        scroll_.SetRange(0, 100); scroll_.SetValue(31);
        static_cast<void>(mwtl::InitializeFlatScrollBars(scroll_.GetHwnd()));
        const std::array status_parts{120, -1};
        static_cast<void>(status_bar_.SetParts(status_parts));
        require(static_cast<bool>(mwtl::SetPartTexts(status_bar_, {{0, L"ready"}})),
                "Status text population failed");
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
        if (!event.Is(button_, NM_CLICK)) return mwtl::EventResult::Propagate();
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
