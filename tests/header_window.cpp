#include <mwtl/window.h>

class HeaderWindow final : public mwtl::Window<HeaderWindow> {
public:
    void BuildUI() {}
};

class HeaderSimpleWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {}
};
