#include <mwtl/mwtl.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>

namespace {
constexpr UINT kGreetingMessage = WM_APP + 42;

class NativeMessageWindow final : public mwtl::Window<NativeMessageWindow> {
public:
    void BuildUI() {
        ::SetWindowPos(GetHwnd(), nullptr, 0, 0, 900, 560,
                       SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        if (!SetTitle(L"Native message: posting WM_APP + 42...")) {
            throw std::runtime_error("SetTitle failed");
        }
        if (::PostMessageW(GetHwnd(), kGreetingMessage, 2026, 0) == FALSE) {
            throw std::runtime_error("PostMessageW failed");
        }
    }

    BEGIN_MSG_MAP(NativeMessageWindow)
        MESSAGE_HANDLER(kGreetingMessage, OnGreeting)
        CHAIN_MSG_MAP(mwtl::Window<NativeMessageWindow>)
    END_MSG_MAP()

private:
    LRESULT OnGreeting(UINT, WPARAM year, LPARAM, BOOL&) {
        wchar_t title[128]{};
        _snwprintf_s(title, _countof(title), _TRUNCATE,
                     L"Native WM_APP received (payload: %llu)",
                     static_cast<unsigned long long>(year));
        SetTitle(title);
        return 0;
    }
};
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try { return mwtl::Application(instance).Run<NativeMessageWindow>(show_command); }
    catch (const std::exception& error) { ::OutputDebugStringA(error.what()); }
    catch (...) { ::OutputDebugStringW(L"Unknown exception at wWinMain.\r\n"); }
    return EXIT_FAILURE;
}
