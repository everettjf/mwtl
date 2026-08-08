#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class ResizeWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        if (!SetTitle(L"Resize demo — resize this window")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    mwtl::EventResult OnResize(const mwtl::ResizeEvent& event) override {
        wchar_t title[128]{};
        _snwprintf_s(title, _countof(title), _TRUNCATE,
                     L"WM_SIZE: %ld × %ld",
                     event.client_size.cx, event.client_size.cy);
        SetTitle(title);
        return mwtl::EventResult::Propagate();
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<ResizeWindow>(instance, show_command);
}
