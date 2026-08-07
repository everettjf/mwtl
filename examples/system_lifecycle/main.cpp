#include <mwtl/mwtl.h>

#include <cstdlib>

class SystemWindow final : public mwtl::Window<SystemWindow> {
public:
    void BuildUI() { SetTitle(L"Native system lifecycle messages"); }

    mwtl::EventResult OnMessage(const mwtl::WindowMessage& message) noexcept {
        if (message.id == WM_QUERYENDSESSION) {
            return mwtl::EventResult::Handled(TRUE);
        }
        if (message.id == WM_GETOBJECT) {
            // A consumer can return UiaReturnRawElementProvider(...) here.
            return mwtl::EventResult::Propagate();
        }
        const bool lifecycle_message =
            message.id == WM_DISPLAYCHANGE || message.id == WM_SETTINGCHANGE ||
            message.id == WM_POWERBROADCAST ||
            message.id == WM_IME_STARTCOMPOSITION ||
            message.id == WM_IME_COMPOSITION || message.id == WM_ENDSESSION;
        if (!lifecycle_message) {
            return mwtl::EventResult::Propagate();
        }
        wchar_t title[96]{};
        ::swprintf_s(title, L"Native lifecycle message: 0x%04X", message.id);
        SetTitle(title);
        return mwtl::EventResult::Propagate();
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<SystemWindow>(
        instance,
        show_command,
        {},
        {.com_apartment = mwtl::ComApartment::sta});
}
