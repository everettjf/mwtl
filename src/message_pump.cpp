#include <mwtl/message_pump.h>

#include "detail/diagnostics.h"

#include <cstdlib>
#include <exception>

namespace mwtl {

WaitAwareMessagePump::WaitAwareMessagePump(WaitAwarePumpOptions options) noexcept
    : options_(options) {}

int WaitAwareMessagePump::Run(WTL::CMessageLoop& wtl_loop) noexcept {
    if (options_.handle_count > MAXIMUM_WAIT_OBJECTS - 1 ||
        (options_.handle_count != 0 && options_.handles == nullptr)) {
        detail::ReportWin32(L"wait-aware message pump options", ERROR_INVALID_PARAMETER, false);
        return EXIT_FAILURE;
    }

    try {
        MSG message{};
        for (;;) {
            while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != FALSE) {
                if (message.message == WM_QUIT) {
                    return static_cast<int>(message.wParam);
                }
                if (wtl_loop.PreTranslateMessage(&message) == FALSE) {
                    ::TranslateMessage(&message);
                    ::DispatchMessageW(&message);
                }
                if (options_.delegate != nullptr) {
                    options_.delegate->AfterDispatch(message);
                }
            }

            DWORD timeout = options_.idle_timeout_ms;
            if (options_.delegate != nullptr) {
                const DWORD delegate_timeout = options_.delegate->GetWaitTimeout();
                if (timeout == INFINITE || delegate_timeout < timeout) {
                    timeout = delegate_timeout;
                }
            }

            const DWORD result = ::MsgWaitForMultipleObjectsEx(
                static_cast<DWORD>(options_.handle_count),
                options_.handles,
                timeout,
                QS_ALLINPUT,
                MWMO_INPUTAVAILABLE);

            if (result == WAIT_FAILED) {
                detail::ReportWin32(L"MsgWaitForMultipleObjectsEx", ::GetLastError(), false);
                return EXIT_FAILURE;
            }
            if (result == WAIT_TIMEOUT) {
                if (options_.delegate != nullptr) {
                    options_.delegate->OnIdle();
                }
                continue;
            }
            const DWORD first_handle = WAIT_OBJECT_0;
            const DWORD after_handles = first_handle +
                static_cast<DWORD>(options_.handle_count);
            if (result >= first_handle && result < after_handles) {
                if (options_.delegate != nullptr) {
                    options_.delegate->OnHandleSignaled(
                        static_cast<std::size_t>(result - first_handle));
                }
                continue;
            }
            if (result != after_handles) {
                detail::ReportWin32(L"wait-aware message pump result", ERROR_INVALID_DATA, false);
                return EXIT_FAILURE;
            }
        }
    } catch (const std::exception& error) {
        detail::ReportException(L"wait-aware message pump callback", 0, error.what(), false);
    } catch (...) {
        detail::ReportUnknownException(L"wait-aware message pump callback", 0, false);
    }
    return EXIT_FAILURE;
}

}  // namespace mwtl
