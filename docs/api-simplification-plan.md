# mwtl API simplification decisions

Status: implemented for 0.4.0. This document records the design constraints and
the reasons behind the selected surface.

This plan keeps mwtl a thin retained-mode Win32/WTL layer. Short syntax is a
default path, not a replacement for explicit ownership, options, return values,
or native handles.

## Design constraints

1. A control remains a named C++ object whose lifetime is visible.
2. Every convenience API has an explicit `Options` path.
3. Existing non-throwing `Create` and raw `HWND` access remain available.
4. Convenience must preserve useful failure information.
5. One pattern must cover all 27 `NativeControl` types; no basic/specialized
   split in the public syntax.
6. Strong IDs and DIP units remain strong types.

## Recommended creation syntax

The current concise path is structurally sound:

```cpp
mwtl::ControlHost ui{*this};
ui.Add(name_, kName, L"mwtl developer");
ui.Add(accent_, kAccent, combo_bounds, combo_options).AddItem(L"Blue");
```

Use it for every child control, including `TreeView`, `DateTimePicker`,
`Toolbar`, `Pager`, and `StatusBar`. `ControlHost::AddNative` remains the
escape hatch for unusual future signatures. Direct `Create` remains the
non-throwing path.

Do not add a builder that owns controls internally. It would shorten member
declarations at the cost of making C++/HWND lifetime, subclassing, and event
references less obvious.

## Geometry

The largest remaining call-site noise is the nested aggregate form:

```cpp
{{8_dip, 40_dip}, {180_dip, 28_dip}}
```

Version 0.4 adds a four-value `RectDip` construction path so the same value can
be written as:

```cpp
{8_dip, 40_dip, 180_dip, 28_dip}
```

The project chose the constructor for the pre-1.0 API. The original nested
point/size construction remains valid, but `RectDip` is no longer an aggregate
and designated aggregate initialization is intentionally not supported. The
API does not accept untyped integers: `_dip` keeps physical units visible.

The larger solution is the 0.4 layout layer. It computes `RectDip` values while
controls remain normal members and covers rows, columns, overlays, spacing,
margins, minimum/maximum sizes, stretch weights, DPI, and resize.

## Events

Version 0.4 adds control-aware matching consistently:

```cpp
if (event.Is(accent_, CBN_SELCHANGE)) { /* ... */ }
if (event.Is(button_, BN_CLICKED)) { /* ... */ }
```

This removes comparisons against both `event.control` and `event.id` while
still exposing those native fields. Keep `IsClicked` as the common button
shortcut. Add parallel `NotifyEvent::IsFrom(control)` and
`NotifyEvent::Is(control, code)` helpers without hiding `NMHDR`.

## Repeated population and configuration

Version 0.4 provides checked batch helpers for item controls rather than making
every setter fluent:

```cpp
mwtl::AddItems(combo_, {L"Blue", L"Green", L"Violet"});
```

The helper must stop and report the first `CB_ERR`/`LB_ERR`; silently ignoring
the current integer results is not acceptable. Similar typed batches can later
cover list-view columns, tabs, toolbar buttons, and status-bar parts.

Do not change existing `void` or `bool` setters to return `Control&` merely for
chaining. That would blur error-bearing and infallible operations and create
avoidable ABI churn. A future `Configure(control, operations...)` helper is
preferable if real examples demonstrate enough value.

## Resources and desktop APIs

`Menu`, `AcceleratorTable`, `ImageList`, `Tooltip`, `UiFont`, and `UiTimer`
have ownership or failure behavior different from child controls. They should
not be forced into `ControlHost`. If repeated checked creation remains noisy,
introduce a small `ThrowLastError(operation, context)` utility rather than a
second generic builder.

Keep dialogs and clipboard operations as result-returning free functions.
Their cancellation/error states do not fit fluent syntax. Keep registry and
placement APIs explicit about root, subkey, and value name.

## Application and window syntax

Keep `RunApplication<Window>` as the one-line default and `Application` as the
advanced path for COM and custom pumps. Do not add global application state.
Keep the CRTP `Window<T>` and virtual `WindowBase` alternatives: they serve
zero-overhead and runtime-polymorphic users respectively.

## Staged implementation

### 0.4 delivered

- all 27 control families in examples/tests use `ControlHost`;
- command and notify events have control-aware matchers;
- compile/runtime coverage proves concise and direct creation paths;
- four-DIP geometry is available with a documented aggregate tradeoff;
- checked item/column/tab/button/status batches report partial progress;
- responsive row/column/overlay layout is public and automatically attached to
  `Window` resize handling.
- the C++20 usability pass adds public concepts, range-aware batches,
  `std::source_location` failure adapters, zero-provisional-bounds control
  creation, rvalue-qualified layout builders, and window-owned `UseLayout`.

### Later: only with demonstrated demand

- declarative layout descriptions;
- bindings or observable properties;
- coroutine event adapters.

These should remain out of the core until their ownership, threading, error,
and native-message behavior are as explicit as today's API.

## Acceptance gates

- every public header compiles independently under C++20;
- all 27 controls compile and create through both `ControlHost` and direct
  `Create` paths;
- existing options structs and native handles remain reachable;
- negative-path tests retain Win32 error information;
- Debug and Release x64 CI, installed-package consumer, and runtime tests pass;
- concise examples are materially shorter without hiding lifetimes.
