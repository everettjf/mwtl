#include <mwtl/mwtl.h>

#include <atomic>
#include <cstdlib>
#include <stdexcept>

namespace {

using mwtl::operator""_dip;

constexpr UINT kRenderDirty = WM_APP + 17;
constexpr LPARAM kUiaRootObjectId = -25;
enum MessageBit : unsigned long long {
    size_bit = 1ull << 0,
    paint_bit = 1ull << 1,
    minmax_bit = 1ull << 2,
    power_bit = 1ull << 3,
    settings_bit = 1ull << 4,
    display_bit = 1ull << 5,
    char_bit = 1ull << 6,
    ime_bit = 1ull << 7,
    key_bit = 1ull << 8,
    mouse_bit = 1ull << 9,
    cursor_bit = 1ull << 10,
    timer_bit = 1ull << 11,
    dirty_bit = 1ull << 12,
    accessibility_bit = 1ull << 13,
    query_end_bit = 1ull << 14,
    end_session_bit = 1ull << 15,
};
constexpr unsigned long long kExpectedMessages = (1ull << 16) - 1;
std::atomic<unsigned long long> observed_messages{0};

struct LineyClassTraits {
    static const wchar_t* GetClassName() noexcept { return L"mwtl.LineyHostCompat"; }
    static UINT GetClassStyle() noexcept { return CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; }
    static HICON GetIcon() noexcept { return nullptr; }
    static HICON GetSmallIcon() noexcept { return nullptr; }
    static HCURSOR GetCursor() noexcept { return ::LoadCursorW(nullptr, IDC_ARROW); }
    static HBRUSH GetBackground() noexcept { return nullptr; }
};

class LineyHost final : public mwtl::Window<LineyHost, LineyClassTraits> {
public:
    using BaseWindow = mwtl::Window<LineyHost, LineyClassTraits>;
    void BuildUI() {
        MINMAXINFO minmax{};
        ::SendMessageW(GetHwnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM(960, 640));
        ::SendMessageW(GetHwnd(), WM_PAINT, 0, 0);
        ::SendMessageW(GetHwnd(), WM_GETMINMAXINFO, 0, reinterpret_cast<LPARAM>(&minmax));
        ::SendMessageW(GetHwnd(), WM_POWERBROADCAST, PBT_APMRESUMEAUTOMATIC, 0);
        ::SendMessageW(GetHwnd(), WM_SETTINGCHANGE, SPI_SETHIGHCONTRAST, 0);
        ::SendMessageW(GetHwnd(), WM_DISPLAYCHANGE, 32, MAKELPARAM(1920, 1080));
        ::SendMessageW(GetHwnd(), WM_CHAR, L'x', 0);
        ::SendMessageW(GetHwnd(), WM_IME_COMPOSITION, 0, GCS_RESULTSTR);
        ::SendMessageW(GetHwnd(), WM_KEYDOWN, VK_RETURN, 0);
        ::SendMessageW(GetHwnd(), WM_MOUSEMOVE, 0, MAKELPARAM(10, 20));
        ::SendMessageW(GetHwnd(), WM_SETCURSOR, reinterpret_cast<WPARAM>(GetHwnd()), 0);
        ::SendMessageW(GetHwnd(), WM_TIMER, 1, 0);
        ::SendMessageW(GetHwnd(), kRenderDirty, 0, 0);
        const LRESULT provider = ::SendMessageW(
            GetHwnd(), WM_GETOBJECT, 0, kUiaRootObjectId);
        if (provider != 0x1234) {
            throw std::runtime_error("WM_GETOBJECT provider hand-off failed");
        }
        if (::SendMessageW(GetHwnd(), WM_QUERYENDSESSION, 0, 0) != TRUE) {
            throw std::runtime_error("WM_QUERYENDSESSION result was not preserved");
        }
        ::SendMessageW(GetHwnd(), WM_ENDSESSION, FALSE, 0);
        ::PostMessageW(GetHwnd(), WM_CLOSE, 0, 0);
    }

    BEGIN_MSG_MAP(LineyHost)
        MESSAGE_RANGE_HANDLER(WM_KEYFIRST, WM_KEYLAST, OnNativeMessage)
        MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnNativeMessage)
        MESSAGE_HANDLER(WM_SIZE, OnNativeMessage)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, OnNativeMessage)
        MESSAGE_HANDLER(WM_POWERBROADCAST, OnNativeMessage)
        MESSAGE_HANDLER(WM_SETTINGCHANGE, OnNativeMessage)
        MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnNativeMessage)
        MESSAGE_HANDLER(WM_IME_COMPOSITION, OnNativeMessage)
        MESSAGE_HANDLER(WM_SETCURSOR, OnNativeMessage)
        MESSAGE_HANDLER(WM_TIMER, OnNativeMessage)
        MESSAGE_HANDLER(kRenderDirty, OnNativeMessage)
        MESSAGE_HANDLER(WM_GETOBJECT, OnGetObject)
        MESSAGE_HANDLER(WM_QUERYENDSESSION, OnQueryEndSession)
        MESSAGE_HANDLER(WM_ENDSESSION, OnNativeMessage)
        CHAIN_MSG_MAP(BaseWindow)
    END_MSG_MAP()

private:
    LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL&) noexcept {
        PAINTSTRUCT paint{};
        const HDC dc = ::BeginPaint(GetHwnd(), &paint);
        if (dc != nullptr) ::EndPaint(GetHwnd(), &paint);
        observed_messages.fetch_or(paint_bit, std::memory_order_relaxed);
        return 0;
    }

    LRESULT OnNativeMessage(UINT message, WPARAM, LPARAM, BOOL& handled) noexcept {
        unsigned long long bit = 0;
        switch (message) {
        case WM_SIZE: bit = size_bit; break;
        case WM_GETMINMAXINFO: bit = minmax_bit; break;
        case WM_POWERBROADCAST: bit = power_bit; break;
        case WM_SETTINGCHANGE: bit = settings_bit; break;
        case WM_DISPLAYCHANGE: bit = display_bit; break;
        case WM_CHAR: bit = char_bit; break;
        case WM_IME_COMPOSITION: bit = ime_bit; break;
        case WM_KEYDOWN: bit = key_bit; break;
        case WM_MOUSEMOVE: bit = mouse_bit; break;
        case WM_SETCURSOR: bit = cursor_bit; break;
        case WM_TIMER: bit = timer_bit; break;
        case kRenderDirty: bit = dirty_bit; break;
        case WM_ENDSESSION: bit = end_session_bit; break;
        default: break;
        }
        observed_messages.fetch_or(bit, std::memory_order_relaxed);
        if (message == WM_SETCURSOR || message == WM_GETMINMAXINFO) handled = FALSE;
        return 0;
    }

    LRESULT OnGetObject(UINT, WPARAM, LPARAM object_id, BOOL& handled) noexcept {
        if (object_id == kUiaRootObjectId) {
            observed_messages.fetch_or(accessibility_bit, std::memory_order_relaxed);
            return 0x1234;
        }
        handled = FALSE;
        return 0;
    }

    LRESULT OnQueryEndSession(UINT, WPARAM, LPARAM, BOOL&) noexcept {
        observed_messages.fetch_or(query_end_bit, std::memory_order_relaxed);
        return TRUE;
    }
};

}  // namespace

int main() {
    mwtl::WindowOptions options;
    options.use_default_bounds = false;
    options.initial_bounds = {{0.0_dip, 0.0_dip}, {960.0_dip, 640.0_dip}};
    options.bounds_are_client_size = true;
    options.center_in_work_area = true;
    mwtl::WaitAwareMessagePump pump({nullptr, 0, 16, nullptr});
    const int result = mwtl::Application(::GetModuleHandleW(nullptr)).Run<LineyHost>(
        SW_HIDE, options, pump);
    return result == 0 && observed_messages.load(std::memory_order_acquire) ==
        kExpectedMessages ? 0 : 1;
}
