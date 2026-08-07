#include <mwtl/mwtl.h>

#include <utility>

static_assert(mwtl::Dip{1.0f} < mwtl::Dip{2.0f});

int main() {
    const auto dpi = mwtl::DpiContext::FromDpi(144);
    const mwtl::RectDip concise{
        mwtl::Dip{1.0f}, mwtl::Dip{2.0f},
        mwtl::Dip{3.0f}, mwtl::Dip{4.0f}};
    auto root = mwtl::Column();
    root.Margin(mwtl::Dip{8.0f});
    mwtl::LayoutHost layout(std::move(root));
    return dpi.ToPixels(mwtl::Dip{2.0f}) == 3 &&
           concise.size.width == mwtl::Dip{3.0f} && layout.HasRoot()
        ? 0 : 1;
}
