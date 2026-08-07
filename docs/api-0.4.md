# mwtl 0.4 concise APIs and layout

Version 0.4 makes the short path consistent across every native child control
without removing direct Win32-oriented APIs.

## Creation and geometry

`ControlHost` binds a parent once and throws `std::system_error` if native
creation fails. It supports all 27 child-control wrappers, including nested
parents:

```cpp
mwtl::ControlHost ui{*this};
ui.Add(name_, kName, L"Developer"); // Layout will assign the bounds.
ui.Add(rebar_, kRebar, {16_dip, 60_dip, 600_dip, 40_dip});

mwtl::ControlHost rebar_ui{rebar_};
rebar_ui.Add(toolbar_, kToolbar, {0_dip, 0_dip, 400_dip, 32_dip});
```

The four-DIP `RectDip` constructor is `x, y, width, height`. The original
`{{x, y}, {width, height}}` form remains valid. Code that requires non-throwing
handling can continue to call `control.Create(...)` directly.

The no-bounds overload creates the native control with a zero provisional
rectangle. Use it when `UseLayout` will arrange the control before the window
is shown. Explicit bounds remain the right choice for manually positioned UI.

`WindowLike` describes any type with `GetHwnd() -> HWND`; `ControlLike` adds a
control ID. The public concepts let application wrappers participate without
inheriting from an mwtl implementation class.

## Event matching

Control-aware matching verifies both the native source `HWND` and code:

```cpp
if (event.Is(theme_, CBN_SELCHANGE)) { /* ... */ }
if (event.IsClicked(save_)) { /* ... */ }
if (notify.Is(tree_, TVN_SELCHANGED)) { /* ... */ }
```

The public `id`, `notification`, `control`, and `NMHDR` remain available.

## Checked batches

Batch helpers stop at the first native failure:

```cpp
const mwtl::BatchResult result =
    mwtl::AddItems(theme_, {L"System", L"Light", L"Dark"});
if (!result) {
    // result.completed, result.failed_at, result.native_result
}
```

The same result contract applies to `AddColumns`, `AddTabs`, `AddButtons`, and
`SetPartTexts`. Each helper accepts initializer lists, spans, or any compatible
C++20 `input_range`, including views:

```cpp
auto visible = names | std::views::filter(IsVisible);
mwtl::Must(mwtl::AddItems(list_, visible), "populate visible names");
```

`Must(bool)` converts a checked Win32-style failure to `std::system_error`.
`Must(BatchResult)` reports the failure index, completed count, and native
result. Both include the C++20 `std::source_location` of the call site.

## Responsive layout

Controls stay ordinary named C++ members. Layout nodes only hold non-owning
`HWND` references:

```cpp
UseLayout(
    mwtl::Row()
        .Margin(16_dip)
        .Gap(12_dip)
        .Add(navigation_, mwtl::Fixed(220_dip))
        .Add(
            mwtl::Column()
                .Gap(8_dip)
                .Add(name_, mwtl::Fixed(32_dip))
                .Add(notes_, mwtl::Stretch(1.0f, 80_dip)),
            mwtl::Stretch()));
```

`UseLayout` makes the window own the layout, performs the initial arrangement,
and automatically rearranges on non-minimized `WM_SIZE`. Rvalue-qualified
builders keep nested move-only nodes in one expression. `SetLayout(LayoutHost&)`
and `LayoutHost::Arrange` remain public for externally owned or unusual hosts.

### Tracks and containers

- `Auto(min, max)` uses the current native or explicit preferred size and can
  shrink to its minimum.
- `Fixed(size)` reserves an exact main-axis size.
- `Stretch(weight, min, max)` distributes remaining main-axis space by weight.
- `Row`, `Column`, and `Overlay` can be nested.
- `Margin`, `Gap`, and `CrossAlignment` use DIP units.
- `LayoutItemOptions::preferred_size` controls measurement.
- `LayoutItemOptions::native_size` preserves hidden native extents such as a
  combo box drop-down height without changing the allocated track.

All final HWND positions are converted using the parent's current DPI and
applied with deferred positioning when available.

## Ownership boundary

`Menu`, `AcceleratorTable`, `Tooltip`, `ImageList`, `UiFont`, dialogs, and
desktop helpers retain their existing APIs. They have ownership, cancellation,
or error behavior that does not match child-control creation and are therefore
not hidden behind `ControlHost` or layout nodes.
