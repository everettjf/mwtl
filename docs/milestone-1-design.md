# Milestone 1 design

Milestone 1 establishes the smallest usable WTL/WIL application path. It intentionally does not define a general multi-window application model.

## Module and application lifetime

The single process-wide WTL adapter `_Module` is defined in `src/module.cpp`; its only declaration is the internal `src/detail/module.h`. `mwtl::Application` owns the pairing of `_Module.Init` and `_Module.Term` and does not store application services in `_Module`.

`Application::Run` initializes in this order: reject re-entry, initialize `_Module`, register its stack-owned `WTL::CMessageLoop`, construct the stack-owned main-window object, create its HWND, show/update the HWND, then run the loop. A WIL scope guard calls `EndRun` on every return or caught-exception path. Cleanup removes the message loop before terminating `_Module`. A failed `_Module.Init` is immediately balanced with `Term` because WTL can partially initialize its ATL base before a later allocation fails. A failed loop registration terminates the initialized module. A failed window creation destroys any partially created HWND through Win32's creation failure path, then unregisters the loop and terminates the module.

## Main window ownership and exit

The `MainWindow` C++ object lives on the `Application::Run` stack and cannot be copied or moved because WTL's thunk and HWND state refer to its stable address. Win32/WTL owns creation and destruction of the HWND; the C++ object only observes it. `GetHwnd()` exposes that native handle and callers may use normal APIs such as `SendMessageW` directly.

`Window<T>` is the milestone-1 main top-level window base. `BuildUI` runs during `WM_CREATE`, after WTL has attached the HWND and before normal `WM_CREATE` message-map dispatch. Returning from `BuildUI` does not imply that later message-map processing accepted creation. The default `WM_CLOSE` path reaches `DefWindowProc` and destroys the window. On `WM_DESTROY`, the boundary posts `WM_QUIT`; this is deliberately a single-main-window exit policy. WTL clears `m_hWnd` after outermost `WM_NCDESTROY`, so `GetHwnd()` returns null and `IsWindow()` is false afterward. The exceptional `WM_NCDESTROY` recovery path also clears it explicitly.

## ABI exception boundary

`Window<T>::GetWindowProc` supplies a `noexcept` thunk target that wraps WTL's actual `CFrameWindowImpl::WindowProc`. Consequently the boundary surrounds `BuildUI`, the derived WTL message map, chained WTL handlers, and default processing initiated by that dispatch. It separately reports `std::exception` and unknown exceptions with the stage and message number.

- During creation, an exception reports a startup error and returns the Win32 creation-failure result (`FALSE` for pre-`WM_CREATE`, `-1` for `WM_CREATE`). `Application::Run` then returns a nonzero code.
- During an ordinary message, it records the failure and posts `WM_CLOSE`. Recovery dispatch bypasses user handling and calls `DestroyWindow`; it does not destroy recursively inside the failing handler.
- During `WM_DESTROY`, it records the failure and posts a nonzero `WM_QUIT` code. During `WM_NCDESTROY`, it calls only `DefWindowProcW`, invalidates the stored HWND, and posts the nonzero quit code; it does not recursively destroy.

The Windows SDK declares `wWinMain` without `noexcept`, so the hello definition matches that ABI declaration and contains a final `std::exception`/unknown catch boundary for construction or startup omissions. No exception can leave its body. Diagnostics use fixed buffers, preserve the incoming last-error value, write to `OutputDebugStringW`, and show a short message box only for startup failures.

When tests are enabled, a private `MWTL_TESTING` build path suppresses startup message boxes only when the test process sets `MWTL_TEST_NO_DIALOGS`. It also records lifecycle counters and permits deterministic module-init and loop-registration failure injection. None of these mechanisms is part of the public include tree or consumer-facing API. CTest uses them to prove the cleanup counts for module-init failure, loop-registration failure, successful creation, creation failure, and message-dispatch failure.

## Deferred work

DIP/geometry types, `WM_DPICHANGED`, layout, controls, themes, fonts, dispatcher/concurrency, Windows 11 effects, Direct2D, DirectWrite, and diagnostics examples are explicitly deferred. The manifest establishes Per-Monitor V2 awareness but milestone 1 performs no DPI-driven relayout.
