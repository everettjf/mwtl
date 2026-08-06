#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

namespace {

constexpr UINT kShowNativeMessage = WM_APP + 1;

class WindowDemo final : public mwtl::Window<WindowDemo> {
public:
    void BuildUI() {
        if (!SetTitle(L"mwtl::Window demo — click the client area or press Esc")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    BEGIN_MSG_MAP(WindowDemo)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLeftButtonDown)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(kShowNativeMessage, OnNativeMessage)
        CHAIN_MSG_MAP(mwtl::Window<WindowDemo>)
    END_MSG_MAP()

private:
    LRESULT OnLeftButtonDown(UINT, WPARAM, LPARAM, BOOL&) {
        ::SendMessageW(GetHwnd(), kShowNativeMessage, 0, 0);
        return 0;
    }

    LRESULT OnKeyDown(UINT, WPARAM key, LPARAM, BOOL& handled) {
        if (key == VK_ESCAPE) {
            ::SendMessageW(GetHwnd(), WM_CLOSE, 0, 0);
        } else {
            handled = FALSE;
        }
        return 0;
    }

    LRESULT OnNativeMessage(UINT, WPARAM, LPARAM, BOOL&) {
        SetTitle(L"Native WM_APP message received — press Esc to close");
        return 0;
    }
};

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try {
        return mwtl::Application(instance).Run<WindowDemo>(show_command);
    } catch (const std::exception& error) {
        ::OutputDebugStringA(error.what());
    } catch (...) {
        ::OutputDebugStringW(L"Unknown exception at the wWinMain boundary.\r\n");
    }
    return EXIT_FAILURE;
}
