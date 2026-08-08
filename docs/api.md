# Current mwtl API

The API prioritizes concise, safe application code over historical source
compatibility.

## Concise ownership defaults

`ControlHost::Add(control, ...)` allocates a host-local control ID starting
at `0x4000`; pass a `ControlId` only when a stable resource ID is required.
`WindowBase::SetLayout(LayoutNode)` owns the layout tree, so there is no
non-owning layout attachment whose lifetime the caller must manage. Layout
measurement stores HWND values rather than references to movable wrappers.

Open, save, and folder pickers use the Vista-era `IFileDialog` family. Their
options use strings, booleans, and `std::filesystem::path` instead of legacy
hook callbacks and `OPENFILENAME`/`BROWSEINFO` flags.

## Layout and controls

All built-in controls expose dynamic intrinsic measurement. `Auto()` remeasures
whenever layout is measured, so text, content, font, theme, and DPI changes are
reflected without rebuilding the layout. Operations that may allocate can throw
instead of terminating through an invalid `noexcept` contract.

Native child controls remember their creating thread. Debug builds assert when
a control is used or destroyed from another thread. Use `WindowWakeup` for
cross-thread notification.

Public APIs do not use `[[nodiscard]]`; ordinary concise code needs no casts to
ignore native results.

## Startup dependency injection

`RunApplication` forwards constructor arguments to the main window:

```cpp
return mwtl::RunApplication<MainWindow>(
    instance, show, {.title = L"App"}, {}, settings, service);
```

The window does not need to be default-constructible.

The same constructor forwarding works with a custom message pump through the
consistent `Run(show, options, pump, arguments...)` member overload.

## Wait-aware message pump

The pump uses `std::chrono::milliseconds`, owns a copy of its handle list, and
accepts callbacks directly. There is no delegate base class to implement:

```cpp
using namespace std::chrono_literals;
mwtl::WaitAwareMessagePump pump({
    .handles = handles,
    .idle_interval = 16ms,
    .on_signal = [&](std::size_t index) { HandleSignal(index); },
    .on_idle = [&] { RenderIdleWork(); },
});
```

## Paths and file filters

Dialogs use `std::filesystem::path` and structured filters instead of exposing
the double-NUL Win32 filter encoding:

```cpp
auto selected = mwtl::ShowOpenFileDialog({
    .owner = GetHwnd(),
    .title = L"Open image",
    .filters = {
        {L"Images", L"*.png;*.jpg"},
        {L"All files", L"*.*"},
    },
});
```
