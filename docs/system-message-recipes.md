# Native system-message recipes

mwtl intentionally leaves system semantics with the application. Add handlers
to the derived WTL map and chain to the exact `Window` specialization:

```cpp
BEGIN_MSG_MAP(MainWindow)
    MESSAGE_HANDLER(WM_POWERBROADCAST, OnPower)
    MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettings)
    MESSAGE_HANDLER(WM_GETOBJECT, OnGetObject)
    CHAIN_MSG_MAP(mwtl::Window<MainWindow>)
END_MSG_MAP()
```

Use `handled = FALSE` when `DefWindowProcW` must retain its native behavior.
Preserve the documented LRESULT for messages such as `WM_QUERYENDSESSION` and
`WM_GETOBJECT` rather than reducing handlers to notifications.

| Message | Consumer responsibility |
|---|---|
| `WM_PAINT`, `WM_SIZE` | own renderer resources, resize and invalidation |
| `WM_DPICHANGED` | mwtl applies the suggested rect by default; refresh consumer DPI resources |
| `WM_DISPLAYCHANGE` | re-query monitor/display capabilities |
| `WM_SETTINGCHANGE` | re-query high contrast and other relevant settings |
| `WM_POWERBROADCAST` | pause/resume consumer devices and background services |
| `WM_GETMINMAXINFO` | return native pixel tracking constraints for current DPI |
| `WM_IME_*`, `WM_CHAR` | own candidate position and committed UTF-16 input |
| `WM_SETCURSOR`, mouse wheel | own hit testing and screen/client coordinate conversion |
| `WM_GETOBJECT` | return the consumer-owned UI Automation provider |
| `WM_QUERYENDSESSION`, `WM_ENDSESSION` | save product state and return the required consent value |

See `examples/system_lifecycle`, `examples/self_drawn_host`, and the C++20
`mwtl_liney_host_compat_test`. All handlers run inside the same exception-safe
WTL WindowProc boundary as other user message handlers.
