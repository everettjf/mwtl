# `mwtl::Window<T>` demo

This executable demonstrates all milestone-1 window integration points:

- derive through the WTL-compatible CRTP form `mwtl::Window<WindowDemo>`;
- initialize the attached HWND in `BuildUI()`;
- read the non-owning HWND with `GetHwnd()`;
- use typed C++20 convention handlers without message-map macros;
- send and receive native `WM_APP` messages without an enclosing abstraction;
- send `WM_CLOSE` directly and let the single-main-window policy end the loop.

Click the client area to send the custom native message. Press Escape to close the window.

Legacy WTL message maps remain supported for alternate map IDs and specialized
chains, but they are no longer required for common window messages.
