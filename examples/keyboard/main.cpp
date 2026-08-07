#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class KeyboardWindow final : public mwtl::Window<KeyboardWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"Keyboard demo — press a key; Escape closes")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    void OnKeyDown(const mwtl::KeyEvent& event) {
        if (event.virtual_key == VK_ESCAPE) {
            Close();
            return;
        }
        wchar_t title[96]{};
        _snwprintf_s(title, _countof(title), _TRUNCATE,
                     L"WM_KEYDOWN virtual key: 0x%02llX",
                     static_cast<unsigned long long>(event.virtual_key));
        SetTitle(title);
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<KeyboardWindow>(instance, show_command);
}
