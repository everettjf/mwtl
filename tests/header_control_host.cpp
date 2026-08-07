#include <mwtl/control_host.h>

void mwtl_header_control_host_smoke() {
    mwtl::ControlHost host(nullptr);
    (void)host.GetParent();
}
