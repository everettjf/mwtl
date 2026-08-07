#include <mwtl/concepts.h>

struct ConceptWindow {
    [[nodiscard]] HWND GetHwnd() const noexcept { return nullptr; }
};

static_assert(mwtl::WindowLike<ConceptWindow>);
