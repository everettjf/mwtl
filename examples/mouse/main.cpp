#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

class MouseWindow final : public mwtl::Window<MouseWindow> {
public:
    void BuildUI() {
        if (!SetTitle(L"Mouse demo — move or click in the client area")) {
            throw std::runtime_error("SetTitle failed");
        }
    }

    void OnMouseMove(const mwtl::MouseEvent& event) {
        ShowPoint(L"WM_MOUSEMOVE", event.position);
    }

    void OnLeftButtonDown(const mwtl::MouseEvent& event) {
        ShowPoint(L"WM_LBUTTONDOWN", event.position);
    }

private:
    void ShowPoint(const wchar_t* event_name, POINT position) {
        wchar_t title[128]{};
        _snwprintf_s(title, _countof(title), _TRUNCATE,
                     L"%s at client point (%ld, %ld)",
                     event_name, position.x, position.y);
        SetTitle(title);
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<MouseWindow>(instance, show_command);
}
