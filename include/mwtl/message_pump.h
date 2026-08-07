#pragma once

#include <windows.h>

#include <atlbase.h>
#include <atlapp.h>

#include <cstddef>
#include <span>

namespace mwtl {

class MessagePump {
public:
    virtual ~MessagePump() = default;
    virtual int Run(WTL::CMessageLoop& wtl_loop) noexcept = 0;
};

class WaitAwarePumpDelegate {
public:
    virtual ~WaitAwarePumpDelegate() = default;
    virtual void AfterDispatch(const MSG&) {}
    virtual void OnIdle() {}
    virtual void OnHandleSignaled(std::size_t) {}
    virtual DWORD GetWaitTimeout() const noexcept { return INFINITE; }
};

struct WaitAwarePumpOptions {
    std::span<const HANDLE> handles{};  // Non-owning.
    DWORD idle_timeout_ms = INFINITE;
    WaitAwarePumpDelegate* delegate = nullptr;  // Non-owning.
};

class WaitAwareMessagePump final : public MessagePump {
public:
    explicit WaitAwareMessagePump(WaitAwarePumpOptions options = {}) noexcept;
    int Run(WTL::CMessageLoop& wtl_loop) noexcept override;

private:
    WaitAwarePumpOptions options_{};
};

}  // namespace mwtl
