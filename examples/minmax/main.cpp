#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class MinMaxWindow final : public mwtl::Window<MinMaxWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"WM_GETMINMAXINFO — minimum 480 × 280 pixels")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    void OnMinMaxInfo(mwtl::MinMaxInfoEvent event) {
        event.info.ptMinTrackSize = {480, 280};
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<MinMaxWindow>(instance, show_command);
}
