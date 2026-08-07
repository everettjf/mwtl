# mwtl 0.2 API and ownership reference

## Ownership

| API/value | Ownership and lifetime |
|---|---|
| `Application::GetInstance()` | non-owning process `HINSTANCE` observation |
| `WindowBase` C++ object | recommended stack-owned window with virtual event hooks; alive through HWND destruction |
| `Window<T>` C++ object | stack-owned by `Application::Run`; alive through HWND destruction |
| `Window<T>::GetHwnd()` | non-owning observation; null after `WM_NCDESTROY` |
| `WindowClassTraits` icons/cursor/brush | non-owning; caller keeps resources valid for class lifetime |
| `WindowOptions` icons/cursor/brush | non-owning; mwtl sends/assigns handles but never destroys them |
| `WaitAwarePumpOptions::handles` | non-owning `std::span` and handles; valid until `Run` returns |
| `WaitAwarePumpOptions::delegate` | non-owning; valid until `Run` returns |
| `WindowWakeup` | copyable weak token; does not own the C++ object or HWND |
| `NativeControl` wrappers | move-only owners of child HWNDs; destruction is idempotent when the parent already destroyed the child |
| `UiTimer` | move-only owner of one HWND/timer-ID pair; destruction calls `KillTimer` while the window remains valid |
| COM apartment | owned by `Application` only when explicitly requested and initialization succeeds |

## DPI values

`Dip`, `PointDip`, `SizeDip`, `RectDip`, and `Thickness` are non-owning values.
`DpiContext::FromWindow(nullptr)` deterministically returns 96 DPI. Scalar
conversion rounds to nearest integer, rounds half values away from zero, clamps
overflow/infinity, and maps NaN to zero. Rectangle edge addition is saturating.

## Window creation

The one-argument `Run<Window>(show_command)` remains the explicit lifetime form.
`RunApplication<Window>(instance, show_command)` is the concise process-entry
helper and retains the same cleanup and exception boundary.
For ordinary applications, inherit `WindowBase` so the class name appears only
once:

```cpp
class MainWindow final : public mwtl::WindowBase {
public:
    void BuildUI() override;
};
```

Its virtual event hooks default to `EventResult::Propagate()`. The CRTP
`Window<T>` form remains available for compile-time dispatch, custom class
traits, and derived WTL message maps.
Pass `WindowOptions` for title, window style, extended style, DIP bounds,
client-versus-outer sizing, centering, resources, and the suggested-DPI-rect
policy. Pass class identity and `CS_*` flags at compile time:

```cpp
class MainWindow final : public mwtl::Window<MainWindow, MyClassTraits> {
    // BuildUI and optional convention handlers
};
```

Class traits are part of the WTL specialization so runtime options cannot
silently mutate class identity after registration.

## Convention dispatch

`Window<T>` uses C++20 `requires` expressions to discover public handlers such
as `OnKeyDown(const KeyEvent&)`, `OnCommand(const CommandEvent&)`,
`OnTimer(TimerId)`, `OnPaint(PaintEvent&)`, and
`OnMessage(const WindowMessage&)`. A `void` handler consumes the message.
`EventResult::Propagate()` delegates to WTL/default processing and
`EventResult::Handled(value)` preserves an explicit native result.

The convention dispatcher is an override of WTL's real
`CMessageMap::ProcessWindowMessage`, so it remains inside `SafeWindowProc`.
Existing WTL maps may override it and chain to `Window<T>`.

## Native controls and timer

Label, Button, TextBox, CheckBox, ComboBox, and ProgressBar wrap standard
system child windows. They accept DIP bounds, own their HWND while valid, and
expose it through `GetHwnd()`. `CommandEvent` retains the native control ID,
notification code, and child HWND. `UiTimer::Start` accepts `TimerId` and
`std::chrono::milliseconds`; `Stop` is idempotent and automatic at destruction.
Controls and timers accept either a native `HWND` or a C++20 window-like object
whose `GetHwnd()` returns `HWND`, so member code normally passes `*this`.
`CommandEvent::IsClicked(control)` matches the originating HWND and
`BN_CLICKED`; `Is(id, notification)` remains available for native-ID matching.
`Window<T>::Close()` sends `WM_CLOSE` and preserves its native `LRESULT`.

## Message pumps

Without an explicit pump, `Application` runs WTL's `CMessageLoop`. With
`WaitAwareMessagePump`, queued messages are drained first, WTL message filters
remain active through `PreTranslateMessage`, and then messages and caller-owned
handles are waited together. The delegate controls timeout/idle work. Exceptions
from delegate calls are diagnosed and converted to a nonzero run result; a
still-live main HWND is destroyed before its stack C++ object leaves scope.

## Wake-up

Copy `GetWakeup()` to a producer thread and call `TryWake()`. Implement
`OnWakeup()` for convention dispatch, or handle `WindowWakeup::Message()` in a
legacy WTL map.
Do not interpret its parameters: WPARAM is a private state cookie used to reject
stale HWND reuse. `TryWake()` returns false after native destruction.
