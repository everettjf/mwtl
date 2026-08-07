#include <mwtl/mwtl.h>

static_assert(mwtl::Dip{1.0f} < mwtl::Dip{2.0f});

int main() {
    const auto dpi = mwtl::DpiContext::FromDpi(144);
    return dpi.ToPixels(mwtl::Dip{2.0f}) == 3 ? 0 : 1;
}
