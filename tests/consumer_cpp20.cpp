#include <mwtl/mwtl.h>

static_assert(mwtl::DpiContext::FromDpi(144).GetDpi() == 144);

class Cpp20ConsumerWindow final : public mwtl::Window<Cpp20ConsumerWindow> {
public:
    void BuildUI() {}
};
