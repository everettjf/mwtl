# mwtl 0.3 desktop APIs

Version 0.3 adds deployable CMake packages and thin helpers for common native
desktop work. Handles remain visible and ownership remains explicit.

## Packaging

Installed builds export `mwtl::mwtl` through `mwtlConfig.cmake`. The config
recreates the pinned `WTL::WTL` and `WIL::WIL` header targets before importing
the library. Both `find_package` and source-tree consumers require C++20.

## Concise control creation

`ControlHost` binds a group of control creations to one parent and converts a
failed native `Create` call into `std::system_error`:

```cpp
mwtl::ControlHost ui{*this};
ui.Add(name_, {102}, L"mwtl developer", name_bounds);
ui.Add(accent_, {105}, combo_bounds, combo_options).AddItem(L"Blue");
```

Controls remain ordinary window members, so their C++ lifetime is visible.
IDs, DIP bounds, option structs, `HWND`, and every control-specific method are
unchanged. Direct `control.Create(...)` remains available for code that wants
non-throwing error handling.

## Menus and accelerators

`Menu` owns an unattached `HMENU`. Attaching a menu to a window transfers
ownership to the window. Moving a popup into a parent menu transfers ownership
to that parent. `AcceleratorTable` owns its `HACCEL`; pass the handle to
`Window<T>::SetAccelerators` and keep the table alive as long as the window.

## System dialogs and desktop exchange

- `ShowOpenFileDialog`, `ShowSaveFileDialog`, and `ShowFolderDialog` retain
  native cancellation and extended-error results.
- `SetClipboardText` and `GetClipboardText` exchange Unicode text.
- `EnableFileDrop` and `ReadDroppedFiles` expose shell file drops without
  hiding the underlying `HDROP` message.

## Placement, DPI fonts, focus, and notifications

- `SavedWindowPlacement` can be captured, restored, and persisted under an
  application-owned registry key. Restore clamps the normal rectangle to the
  nearest current monitor work area, including virtual desktops with negative
  coordinates.
- `UiFont::CreateMessageFont` creates the current Windows message font for a
  requested DPI; `SetControlFont` applies it to native children.
- `FocusNextControl` follows native `WS_TABSTOP` order.
- `NotifyEvent` provides typed access to `WM_NOTIFY` identity and payloads while
  preserving the original `NMHDR` and native result propagation.

## Multi-monitor hot-corners reference

`examples/hot_corners` is a complete utility rather than an isolated API
sample. It has per-monitor/per-corner actions, configurable dwell and pixel
tolerance, tray pause/resume, fullscreen auto-pause, registry persistence, and
a `--self-test` mode that exercises activation while suppressing `SendInput`.
Detection uses each monitor's absolute `rcMonitor`, including negative virtual
desktop coordinates and mixed resolutions.
