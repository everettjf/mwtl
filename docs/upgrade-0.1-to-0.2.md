# Upgrade from 0.1 to 0.2

The current branch requires C++20. Update consumer targets to
`target_compile_features(your_target PRIVATE cxx_std_20)` before adopting the
0.2 APIs. `WaitAwarePumpOptions::handles` is a non-owning `std::span<const
HANDLE>`; arrays convert directly and empty spans represent a message-only
wait.

No source change is required for existing 0.1 applications. The original
`Application(instance).Run<MainWindow>(show_command)` call still selects WTL's
standard loop and default window creation behavior.

Opt in incrementally:

1. Use `DpiContext::FromWindow(GetHwnd())` for DIP/pixel conversion.
2. Pass `WindowOptions` when explicit class-independent styles or initial bounds
   are needed; use the second `Window<T, Traits>` template argument for class
   identity and `CS_*` flags.
3. Pass a `WaitAwareMessagePump` only when messages must be combined with wait
   handles or bounded idle work.
4. Copy `GetWakeup()` to producers instead of copying a bare HWND.
5. Request COM ownership explicitly with `ApplicationOptions`; the default is
   still `ComApartment::none`.

`WindowWakeup::Message()` is a registered process message. A handler should not
interpret its WPARAM/LPARAM; mwtl uses WPARAM as a private lifetime cookie.
