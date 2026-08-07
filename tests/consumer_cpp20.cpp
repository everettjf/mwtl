#include <mwtl/mwtl.h>

#include <compare>
#include <memory>
#include <span>

static_assert(mwtl::DpiContext::FromDpi(144).GetDpi() == 144);

class Cpp20ConsumerWindow final : public mwtl::Window<Cpp20ConsumerWindow> {
public:
    void BuildUI() {}
};

static_assert(mwtl::MainWindow<Cpp20ConsumerWindow>);

class InjectedWindow final : public mwtl::Window<InjectedWindow> {
public:
    explicit InjectedWindow(std::shared_ptr<int> service)
        : service_(std::move(service)) {}
    void BuildUI() { *service_ += 1; }

private:
    std::shared_ptr<int> service_;
};

static_assert(mwtl::MainWindow<InjectedWindow>);
static_assert(!std::default_initializable<InjectedWindow>);

int CompileInjectedStartup(HINSTANCE instance, int show) {
    return mwtl::RunApplication<InjectedWindow>(
        instance, show, {}, {}, std::make_shared<int>(0));
}
static_assert((mwtl::Dip{1.0f} <=> mwtl::Dip{2.0f}) < 0);
static_assert(std::span<const HANDLE>{}.empty());
