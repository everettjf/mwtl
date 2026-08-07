#include <mwtl/layout.h>

void mwtl_header_layout_smoke() {
    auto root = mwtl::Column();
    root.Margin(mwtl::Dip{8.0f}).Gap(mwtl::Dip{4.0f});
    mwtl::LayoutHost layout(std::move(root));
    static_cast<void>(layout.HasRoot());
}
