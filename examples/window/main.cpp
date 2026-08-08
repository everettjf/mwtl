#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

namespace {

constexpr UINT kShowNativeMessage = WM_APP + 1;

class WindowDemo final : public mwtl::WindowBase {
public:
    void BuildUI() override {
        if (!SetTitle(L"mwtl::Window demo — click the client area or press Esc")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    mwtl::EventResult OnLeftButtonDown(const mwtl::MouseEvent&) override {
        ::SendMessageW(GetHwnd(), kShowNativeMessage, 0, 0);
        return mwtl::EventResult::Handled();
    }

    mwtl::EventResult OnKeyDown(const mwtl::KeyEvent& event) override {
        if (event.virtual_key == VK_ESCAPE) {
            Close();
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }

    mwtl::EventResult OnMessage(const mwtl::WindowMessage& message) override {
        if (message.id == kShowNativeMessage) {
            SetTitle(L"Native WM_APP message received — press Esc to close");
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }
};

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<WindowDemo>(instance, show_command);
}
