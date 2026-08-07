# mwtl 0.2 API and ownership reference

## Ownership

| API/value | Ownership and lifetime |
|---|---|
| `Application::GetInstance()` | non-owning process `HINSTANCE` observation |
| `Window<T>` C++ object | stack-owned by `Application::Run`; alive through HWND destruction |
| `Window<T>::GetHwnd()` | non-owning observation; null after `WM_NCDESTROY` |
| `WindowClassTraits` icons/cursor/brush | non-owning; caller keeps resources valid for class lifetime |
| `WindowOptions` icons/cursor/brush | non-owning; mwtl sends/assigns handles but never destroys them |
| `WaitAwarePumpOptions::handles` | non-owning array and handles; valid until `Run` returns |
| `WaitAwarePumpOptions::delegate` | non-owning; valid until `Run` returns |
| `WindowWakeup` | copyable weak token; does not own the C++ object or HWND |
| COM apartment | owned by `Application` only when explicitly requested and initialization succeeds |

## DPI values

`Dip`, `PointDip`, `SizeDip`, `RectDip`, and `Thickness` are non-owning values.
`DpiContext::FromWindow(nullptr)` deterministically returns 96 DPI. Scalar
conversion rounds to nearest integer, rounds half values away from zero, clamps
overflow/infinity, and maps NaN to zero. Rectangle edge addition is saturating.

## Window creation

The one-argument `Run<Window>(show_command)` remains the 0.1-compatible form.
Pass `WindowOptions` for title, window style, extended style, DIP bounds,
client-versus-outer sizing, centering, resources, and the suggested-DPI-rect
policy. Pass class identity and `CS_*` flags at compile time:

```cpp
class MainWindow final : public mwtl::Window<MainWindow, MyClassTraits> {
    // BuildUI and WTL message map
};
```

Class traits are part of the WTL specialization so runtime options cannot
silently mutate class identity after registration.

## Message pumps

Without an explicit pump, `Application` runs WTL's `CMessageLoop`. With
`WaitAwareMessagePump`, queued messages are drained first, WTL message filters
remain active through `PreTranslateMessage`, and then messages and caller-owned
handles are waited together. The delegate controls timeout/idle work. Exceptions
from delegate calls are diagnosed and converted to a nonzero run result; a
still-live main HWND is destroyed before its stack C++ object leaves scope.

## Wake-up

Copy `GetWakeup()` to a producer thread and call `TryWake()`. Handle
`WindowWakeup::Message()` in the WTL message map or observe it after dispatch.
Do not interpret its parameters: WPARAM is a private state cookie used to reject
stale HWND reuse. `TryWake()` returns false after native destruction.
