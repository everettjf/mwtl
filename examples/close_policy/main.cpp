#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class ClosePolicyWindow final : public mwtl::Window<ClosePolicyWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"Close policy demo — close twice to exit")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    mwtl::EventResult OnClose() {
        if (!close_confirmed_) {
            close_confirmed_ = true;
            SetTitle(L"Close requested once — close again to confirm");
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

private:
    bool close_confirmed_ = false;
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<ClosePolicyWindow>(instance, show_command);
}
