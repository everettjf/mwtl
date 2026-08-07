#include <mwtl/input_controls.h>

void ConsumeInputControls() {
    mwtl::DateTimePicker date;
    mwtl::MonthCalendar calendar;
    mwtl::HotKey hot_key;
    mwtl::IpAddress address;
    mwtl::UpDown spin;
    mwtl::SysLink link;
    static_cast<void>(date); static_cast<void>(calendar); static_cast<void>(hot_key);
    static_cast<void>(address); static_cast<void>(spin); static_cast<void>(link);
}
