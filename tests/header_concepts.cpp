#include <mwtl/concepts.h>

struct ConceptWindow {
    HWND GetHwnd() const noexcept { return nullptr; }
};

static_assert(mwtl::WindowLike<ConceptWindow>);
