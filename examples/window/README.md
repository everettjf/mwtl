# `mwtl::Window<T>` demo

This executable demonstrates all milestone-1 window integration points:

- derive through the WTL-compatible CRTP form `mwtl::Window<WindowDemo>`;
- initialize the attached HWND in `BuildUI()`;
- read the non-owning HWND with `GetHwnd()`;
- use a normal WTL `BEGIN_MSG_MAP` and chain to the mwtl base;
- send and receive native `WM_APP` messages without an enclosing abstraction;
- send `WM_CLOSE` directly and let the single-main-window policy end the loop.

Click the client area to send the custom native message. Press Escape to close the window.
