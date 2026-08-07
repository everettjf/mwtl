# liney-win migration spike

## Result

The 0.2 host surface covers liney-win's top-level Win32 shell plumbing without
absorbing its terminal or renderer. The repository's
`mwtl_liney_host_compat_test` is a C++20 consumer that exercises liney's class
style, DIP client bounds, wait-aware loop, and representative native message
surface under the real mwtl WindowProc.

The spike reviewed `everettjf/liney-win` at the public `main` branch on
2026-08-06. Its current `Window.cpp` manually performs `RegisterClassExW`,
`CreateWindowExW`, a GWLP_USERDATA thunk, and a
`PeekMessageW`/`MsgWaitForMultipleObjectsEx` loop. Those four responsibilities
map directly to mwtl 0.2:

| liney-win responsibility | mwtl 0.2 replacement |
|---|---|
| class registration and `CS_DBLCLKS` | `Window<T, ClassTraits>` |
| centered initial client size | `WindowOptions` with DIP bounds |
| GWLP_USERDATA dispatch thunk | WTL CRTP dispatch plus mwtl exception boundary |
| dirty-output wake message | `WindowWakeup` |
| `PeekMessage` + timed wait | `WaitAwareMessagePump` delegate |
| initial/current DPI | `DpiContext` and default `WM_DPICHANGED` policy |
| `CoInitializeEx`/`CoUninitialize` | `ApplicationOptions{ComApartment::sta}` |

## What remains in liney-win

`WM_GETOBJECT`, input/IME, mouse/cursor, clipboard, timers, power/session/theme
messages, painting, D2D/DWrite/D3D11, ConPTY, Ghostty VT, tabs/workspaces and
accessibility providers remain liney-owned native handlers and services. WTL
message maps can call the existing methods without changing their semantics.

## Integration sequence

1. Derive the existing top-level `liney::Window` shell from
   `mwtl::Window<Window, LineyClassTraits>` and move the switch cases into a WTL
   message map while retaining handler bodies.
2. Replace `create/show/runMessageLoop` at `wWinMain` with
   `Application::Run`, `WindowOptions`, and a liney pump delegate whose timeout
   renders dirty frames and runs the existing exit hook on `WM_QUIT`.
3. Replace the process-global wake HWND with a `WindowWakeup` token copied into
   the PTY/render-signal producer.
4. Keep Liney's existing manifest and Windows 11 effects; do not duplicate them
   in mwtl.
5. Run Liney's headless visual, input, accessibility, stress-output, installer,
   and shutdown suites before merging.

The full product repository was not rewritten as part of the library change:
it requires Zig 0.15.2 and Ghostty to build, and those product-owned changes
should land in liney-win as a separately reviewed migration commit. The mwtl
compatibility fixture prevents the required host API from regressing meanwhile.
