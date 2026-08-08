#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class MinMaxWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        if (!SetTitle(L"WM_GETMINMAXINFO — minimum 480 × 280 pixels")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    mwtl::EventResult OnMinMaxInfo(mwtl::MinMaxInfoEvent event) override {
        event.info.ptMinTrackSize = {480, 280};
        return mwtl::EventResult::Handled();
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<MinMaxWindow>(instance, show_command);
}
