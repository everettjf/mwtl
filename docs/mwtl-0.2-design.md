# mwtl 0.2 design

## Boundaries

Version 0.2 is an advanced native top-level-window host. It deliberately does
not own terminal, ConPTY, renderer, layout, theme, or product state. Consumers
remain free to handle every Win32 message and call every HWND API directly.

## DPI and initial bounds

`Dip` and the geometry aggregates are value types. `DpiContext` is an explicit
conversion object; pixel rounding is centralized in `src/dpi.cpp`, uses nearest
integer rounding, and saturates on overflow. `DpiContext::FromWindow` always
queries the current HWND DPI.

For an explicitly sized window, `WindowOptions` chooses a monitor from the
requested origin (or the primary monitor when centering), obtains its effective
DPI, converts the requested DIP size, optionally expands a client size with
`AdjustWindowRectExForDpi`, clamps it to the monitor work area, then centers or
clamps the origin. `Window<T>` applies the `WM_DPICHANGED` suggested rectangle by
default; applications can opt out and still receive the raw message.

## Window class and resources

The second CRTP parameter is a class-traits type. It supplies the class name,
class style, icons, cursor, and background brush. `WindowOptions` supplies
per-run title, window styles, bounds and optional non-owning native resources.
The library never destroys handles supplied by the caller.

## Pump and wake-up

`Application::Run` accepts a `MessagePump`; the default remains WTL's
`CMessageLoop`. `WaitAwareMessagePump` drains all queued messages, calls WTL
`PreTranslateMessage`, dispatches, invokes an optional observation callback,
then waits with `MsgWaitForMultipleObjectsEx` and `MWMO_INPUTAVAILABLE`. A
timeout invokes `OnIdle`; no idle polling occurs before the timeout. Existing
WTL message filters remain active. WTL idle handlers are not invoked by the
custom pump because its explicit delegate owns the wait/idle policy.

`WindowWakeup` holds only a weak reference to a private state block. `TryWake`
loads the current HWND atomically and posts a registered message containing a
state cookie. `Window<T>` rejects stale cookies and clears the HWND before
`WM_NCDESTROY` dispatch. The token therefore neither owns the window nor posts
successfully after its lifetime. `WindowWakeup::Message()` lets a WTL message
map or a pump delegate observe valid wake notifications.

## Failure and cleanup

COM initialization, module initialization, loop registration, window creation,
the pump, and window destruction are ordered and unwound in reverse. A custom
pump callback exception is contained, diagnosed, returns failure, and causes a
still-live HWND to be synchronously destroyed while the stack-owned window
object still exists. The original WindowProc and `wWinMain` ABI boundaries are
unchanged.

Optional COM setup is performed before `_Module.Init`; a successful `S_OK` or
`S_FALSE` is paired with exactly one `CoUninitialize`. `RPC_E_CHANGED_MODE` is a
diagnosed startup failure and is never followed by `CoUninitialize`.
