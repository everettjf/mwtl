#include <mwtl/mwtl.h>

#include <concepts>
#include <string_view>

namespace {

template <typename Control>
concept BoundsControl = requires(
    Control& control, HWND parent, mwtl::ControlId id, mwtl::RectDip bounds) {
    { control.Create(parent, id, bounds) } -> std::same_as<bool>;
};

template <typename Control>
concept TextControl = requires(
    Control& control, HWND parent, mwtl::ControlId id,
    std::wstring_view text, mwtl::RectDip bounds) {
    { control.Create(parent, id, text, bounds) } -> std::same_as<bool>;
};

static_assert(TextControl<mwtl::Label>);
static_assert(TextControl<mwtl::Button>);
static_assert(TextControl<mwtl::TextBox>);
static_assert(TextControl<mwtl::CheckBox>);
static_assert(TextControl<mwtl::RadioButton>);
static_assert(TextControl<mwtl::GroupBox>);
static_assert(TextControl<mwtl::SysLink>);

static_assert(BoundsControl<mwtl::ListBox>);
static_assert(BoundsControl<mwtl::ComboBox>);
static_assert(BoundsControl<mwtl::ProgressBar>);
static_assert(BoundsControl<mwtl::Slider>);
static_assert(BoundsControl<mwtl::ScrollBar>);
static_assert(BoundsControl<mwtl::TreeView>);
static_assert(BoundsControl<mwtl::ListView>);
static_assert(BoundsControl<mwtl::Header>);
static_assert(BoundsControl<mwtl::TabControl>);
static_assert(BoundsControl<mwtl::ComboBoxEx>);
static_assert(BoundsControl<mwtl::DateTimePicker>);
static_assert(BoundsControl<mwtl::MonthCalendar>);
static_assert(BoundsControl<mwtl::HotKey>);
static_assert(BoundsControl<mwtl::IpAddress>);
static_assert(BoundsControl<mwtl::UpDown>);
static_assert(BoundsControl<mwtl::Toolbar>);
static_assert(BoundsControl<mwtl::StatusBar>);
static_assert(BoundsControl<mwtl::Rebar>);
static_assert(BoundsControl<mwtl::Pager>);
static_assert(BoundsControl<mwtl::Animation>);

}  // namespace
