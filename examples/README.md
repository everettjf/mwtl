# mwtl examples

Milestone 1 includes 12 focused executables. Each example is intentionally small enough to read as a complete recipe while still using the real `mwtl::Application` and `mwtl::Window<T>` path.

| Directory | Target | Focus |
|---|---|---|
| `hello` | `mwtl_hello` | Smallest complete mwtl program |
| `application` | `mwtl_application_demo` | `mwtl::Application`, process entry, run result, and instance observation |
| `window` | `mwtl_window_demo` | `mwtl::Window<T>`, HWND access, WTL message maps, and native messages |
| `native_message` | `mwtl_native_message_demo` | Post and receive an application-defined `WM_APP` message |
| `keyboard` | `mwtl_keyboard_demo` | Handle keyboard input and close with Escape |
| `mouse` | `mwtl_mouse_demo` | Read native mouse client coordinates |
| `resize` | `mwtl_resize_demo` | Observe native pixel dimensions and size state |
| `timer` | `mwtl_timer_demo` | Start, receive, and clean up a Win32 timer |
| `paint` | `mwtl_paint_demo` | Draw directly in a native `WM_PAINT` handler |
| `minmax` | `mwtl_minmax_demo` | Apply a minimum tracking size with `WM_GETMINMAXINFO` |
| `close_policy` | `mwtl_close_policy_demo` | Intercept close once, then delegate to the base policy |
| `window_state` | `mwtl_window_state_demo` | Observe restored, minimized, and maximized states |

Configure with `MWTL_BUILD_EXAMPLES=ON`, then build one target or all targets:

```powershell
cmake -S . -B build/x64 -G "Visual Studio 17 2022" -A x64 -DMWTL_BUILD_EXAMPLES=ON
cmake --build build/x64 --config Debug --target mwtl_application_demo
cmake --build build/x64 --config Debug --target mwtl_window_demo
cmake --build build/x64 --config Debug --target mwtl_timer_demo
```

With the repository presets, the equivalent full examples/test build is:

```powershell
cmake --preset vs2022-x64
cmake --build --preset x64-debug
```

All examples use the shared Per-Monitor V2 manifest in `example.manifest`.

Run a target from its configuration directory, for example:

```powershell
./build/x64/examples/paint/Debug/mwtl_paint_demo.exe
./build/x64/examples/native_message/Debug/mwtl_native_message_demo.exe
./build/x64/examples/timer/Debug/mwtl_timer_demo.exe
```

Every example remains within milestone 1: native messages and GDI calls demonstrate HWND interoperability, not new mwtl layout, control, theme, or rendering abstractions.
