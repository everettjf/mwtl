#include <mwtl/mwtl.h>

#include <array>
#include <stdexcept>
#include <system_error>

using mwtl::operator""_dip;

class CommonControlsWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        SetTitle(L"Windows Common Controls gallery");
        const auto require = [](bool created, const char* name) {
            if (!created) throw std::system_error(static_cast<int>(::GetLastError()), std::system_category(), name);
        };
        require(rebar_.Create(*this, {200}, {{16.0_dip, 12.0_dip}, {1160.0_dip, 48.0_dip}}), "Rebar");
        require(toolbar_.Create(rebar_, {201}, {{0.0_dip, 0.0_dip}, {420.0_dip, 34.0_dip}}), "Toolbar");
        require(tree_.Create(*this, {202}, {{16.0_dip, 76.0_dip}, {250.0_dip, 310.0_dip}}), "TreeView");
        require(list_.Create(*this, {203}, {{282.0_dip, 76.0_dip}, {400.0_dip, 180.0_dip}}), "ListView");
        require(header_.Create(*this, {204}, {{282.0_dip, 270.0_dip}, {400.0_dip, 34.0_dip}}), "Header");
        require(tabs_.Create(*this, {205}, {{282.0_dip, 318.0_dip}, {400.0_dip, 68.0_dip}}), "TabControl");
        require(combo_ex_.Create(*this, {206}, {{704.0_dip, 76.0_dip}, {250.0_dip, 150.0_dip}}), "ComboBoxEx");
        require(date_.Create(*this, {207}, {{704.0_dip, 126.0_dip}, {250.0_dip, 34.0_dip}}), "DateTimePicker");
        require(calendar_.Create(*this, {208}, {{970.0_dip, 76.0_dip}, {260.0_dip, 210.0_dip}}), "MonthCalendar");
        require(hot_key_.Create(*this, {209}, {{704.0_dip, 176.0_dip}, {250.0_dip, 34.0_dip}}), "HotKey");
        require(ip_.Create(*this, {210}, {{704.0_dip, 226.0_dip}, {250.0_dip, 34.0_dip}}), "IpAddress");
        require(number_.Create(*this, {211}, L"42", {{704.0_dip, 276.0_dip}, {210.0_dip, 34.0_dip}}), "UpDown buddy");
        require(spin_.Create(*this, {212}, {{914.0_dip, 276.0_dip}, {40.0_dip, 34.0_dip}}), "UpDown");
        require(link_.Create(*this, {213}, L"Read the <a href=\"https://learn.microsoft.com/windows/win32/controls/\">Common Controls docs</a>", {{704.0_dip, 326.0_dip}, {510.0_dip, 38.0_dip}}), "SysLink");
        require(pager_.Create(*this, {214}, {{16.0_dip, 410.0_dip}, {666.0_dip, 54.0_dip}}), "Pager");
        require(pager_text_.Create(pager_, {215}, L"Pager child: content can be wider than its viewport", {{0.0_dip, 0.0_dip}, {820.0_dip, 32.0_dip}}), "Pager child");
        require(animation_.Create(*this, {216}, {{704.0_dip, 382.0_dip}, {120.0_dip, 72.0_dip}}), "Animation");
        require(animation_label_.Create(*this, {217}, L"Animation host\n(resource-driven AVI)", {{836.0_dip, 390.0_dip}, {260.0_dip, 56.0_dip}}), "Animation label");
        require(scroll_.Create(*this, {218}, {{16.0_dip, 486.0_dip}, {666.0_dip, 28.0_dip}}), "ScrollBar");
        require(status_.Create(*this, {219}, {{0.0_dip, 540.0_dip}, {1240.0_dip, 28.0_dip}}), "StatusBar");
        require(tooltip_.Create(rebar_.GetHwnd()), "Tooltip");
        require(images_.Create(16, 16), "ImageList");

        static_cast<void>(toolbar_.AddTextButton(kAbout, L"Task Dialog"));
        static_cast<void>(toolbar_.AddTextButton(kRefresh, L"Refresh"));
        toolbar_.AutoSize();
        static_cast<void>(rebar_.AddBand(toolbar_, L"", 360));
        static_cast<void>(tooltip_.AddTool(toolbar_.GetHwnd(), L"Native Toolbar hosted by a Rebar"));

        const HTREEITEM root = tree_.AddItem(L"Common Controls");
        static_cast<void>(tree_.AddItem(L"Navigation", root));
        static_cast<void>(tree_.AddItem(L"Input", root));
        static_cast<void>(tree_.AddItem(L"Commands", root));
        static_cast<void>(tree_.Expand(root));

        static_cast<void>(list_.AddColumn(L"Control", 170));
        static_cast<void>(list_.AddColumn(L"Ownership", 150));
        const int row = list_.AddItem(L"ListView");
        static_cast<void>(list_.SetSubItem(row, 1, L"Native HWND"));
        list_.SetExtendedListStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
        static_cast<void>(header_.AddColumn(L"Standalone Header", 210));
        static_cast<void>(header_.AddColumn(L"Resizable column", 180));
        static_cast<void>(tabs_.AddTab(L"Tree"));
        static_cast<void>(tabs_.AddTab(L"List"));
        static_cast<void>(combo_ex_.AddItem(L"ComboBoxEx item"));
        static_cast<void>(combo_ex_.AddItem(L"Image-capable item"));
        static_cast<void>(combo_ex_.SetSelection(0));
        hot_key_.SetValue('K', HOTKEYF_CONTROL | HOTKEYF_ALT);
        ip_.SetValue(127, 0, 0, 1);
        spin_.SetBuddy(number_); spin_.SetRange(0, 100); spin_.SetValue(42);
        pager_.SetChild(pager_text_); pager_.SetButtonSize(24);
        scroll_.SetRange(0, 100); scroll_.SetValue(35);
        static_cast<void>(mwtl::InitializeFlatScrollBars(scroll_.GetHwnd()));
        const std::array parts{700, 980, -1};
        static_cast<void>(status_.SetParts(parts));
        static_cast<void>(status_.SetPartText(0, L"All specialized control families are native"));
        static_cast<void>(status_.SetPartText(1, L"Unicode + DPI aware"));
        static_cast<void>(images_.AddIcon(::LoadIconW(nullptr, IDI_INFORMATION)));
    }

    mwtl::EventResult OnCommand(const mwtl::CommandEvent& event) override {
        if (event.id == kAbout) {
            static_cast<void>(mwtl::ShowTaskDialog(GetHwnd(), L"mwtl", L"Native Task Dialog", L"This modal control is wrapped as a function, not a child HWND."));
            return mwtl::EventResult::Handled();
        }
        if (event.id == kRefresh) {
            static_cast<void>(status_.SetPartText(2, L"Toolbar command received"));
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

private:
    static constexpr mwtl::ControlId kAbout{500};
    static constexpr mwtl::ControlId kRefresh{501};
    mwtl::Rebar rebar_; mwtl::Toolbar toolbar_; mwtl::TreeView tree_; mwtl::ListView list_;
    mwtl::Header header_; mwtl::TabControl tabs_; mwtl::ComboBoxEx combo_ex_;
    mwtl::DateTimePicker date_; mwtl::MonthCalendar calendar_; mwtl::HotKey hot_key_;
    mwtl::IpAddress ip_; mwtl::TextBox number_; mwtl::UpDown spin_; mwtl::SysLink link_;
    mwtl::Pager pager_; mwtl::Label pager_text_; mwtl::Animation animation_;
    mwtl::Label animation_label_; mwtl::ScrollBar scroll_; mwtl::StatusBar status_;
    mwtl::Tooltip tooltip_; mwtl::ImageList images_;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    return mwtl::RunApplication<CommonControlsWindow>(instance, show, {.title = L"Windows Common Controls gallery", .initial_bounds = {{0.0_dip, 0.0_dip}, {1260.0_dip, 620.0_dip}}, .use_default_bounds = false});
}
