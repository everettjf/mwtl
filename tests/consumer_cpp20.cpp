#include <mwtl/mwtl.h>

#include <compare>
#include <span>

static_assert(mwtl::DpiContext::FromDpi(144).GetDpi() == 144);

class Cpp20ConsumerWindow final : public mwtl::Window<Cpp20ConsumerWindow> {
public:
    void BuildUI() {}
};

static_assert(mwtl::MainWindow<Cpp20ConsumerWindow>);
static_assert((mwtl::Dip{1.0f} <=> mwtl::Dip{2.0f}) < 0);
static_assert(std::span<const HANDLE>{}.empty());
