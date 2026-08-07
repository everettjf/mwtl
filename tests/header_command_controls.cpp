#include <mwtl/command_controls.h>

void ConsumeCommandControls() {
    mwtl::Toolbar toolbar; mwtl::StatusBar status; mwtl::Rebar rebar;
    mwtl::Pager pager; mwtl::Animation animation;
    static_cast<void>(toolbar); static_cast<void>(status); static_cast<void>(rebar);
    static_cast<void>(pager); static_cast<void>(animation);
}
