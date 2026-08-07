# Changelog

## 0.5.0 - Unreleased

- Removed public `[[nodiscard]]` annotations so concise application code can
  intentionally ignore native results without casts or warning suppression.
- Made intrinsic control measurement dynamic: `Auto()` now remeasures after
  text, font, content, theme, or DPI changes instead of caching at layout-add
  time.
- Corrected layout and preferred-size exception specifications so allocation
  failure cannot silently terminate the process through an invalid `noexcept`.
- Added debug-time UI-thread ownership checks to native child controls.
- Added constructor forwarding to `RunApplication` for direct main-window
  dependency injection without globals or service locators.
- Replaced the wait-aware pump delegate hierarchy with owned handle storage,
  `std::chrono` intervals, and direct callable hooks for dispatch, idle, and
  signals.
- Replaced raw double-NUL file filters and string paths with structured filters
  and `std::filesystem::path` dialog values.
- Added typed scroll events, optional selected indexes, typed hot keys and IP
  addresses, plus range inputs for accelerators and status-bar parts.
- Added warning-as-error builds, MSVC static-analysis CI, release checksums,
  dependency automation, a security policy, and a 0.5 release checklist.

- Unified concise `ControlHost` creation across all 27 native child-control
  wrappers while retaining every direct non-throwing `Create` overload.
- Added control-aware `CommandEvent` and `NotifyEvent` matching that keeps the
  original IDs, notification codes, `HWND`, and `NMHDR` available.
- Added four-DIP `RectDip{x, y, width, height}` construction. The original
  nested point/size form remains valid; `RectDip` is intentionally no longer
  an aggregate.
- Added checked batch population for text items, columns, tabs, toolbar
  buttons, and status-part text, including completed count, failure index, and
  the native failure result.
- Added a retained responsive layout layer with nested rows, columns, overlays,
  margins, gaps, Auto/Fixed/Stretch tracks, minimum and maximum constraints,
  cross-axis alignment, preferred/native sizes, DPI conversion, deferred HWND
  positioning, and automatic `WM_SIZE` integration through `Window::SetLayout`.
- Migrated the controls gallery, Common Controls gallery, Hot Corners utility,
  and full control runtime test to the concise APIs.
- Added public `WindowLike` and `ControlLike` concepts for clearer C++20
  extension contracts and compiler diagnostics.
- Added rvalue-qualified fluent layout construction and `Window::UseLayout`
  so a window can own and install a complete layout expression in one call.
- Let `ControlHost` omit provisional bounds for controls immediately managed
  by layout, while retaining every explicit-bounds overload.
- Added `Must` checked-to-throwing adapters with `std::source_location` and
  detailed batch failure context.
- Extended all checked batch APIs to accept C++20 input ranges and views while
  preserving span and initializer-list calls.

## 0.3.0 - 2026-08-07

- Narrowed the currently supported architecture to x64; ARM64 is deferred until
  it has a maintained validation environment.
- Added an installable CMake package, an installed `find_package` consumer
  regression test, ZIP packaging, and a tag-driven release workflow.
- Added native menus and accelerators, file/folder dialogs, Unicode clipboard,
  shell file drops, persistent monitor-safe placement, DPI message fonts,
  focus navigation, and typed `WM_NOTIFY` helpers.
- Added a complete multi-monitor Hot Corners utility with per-screen/per-corner
  actions, configurable dwell/tolerance, tray and fullscreen pause, persistence,
  and a no-input self-test mode.
- Added `ControlHost`, a concise throwing control-creation syntax that binds the
  parent once while retaining explicit member lifetime, IDs, DIP bounds, native
  option structs, handles, and the existing non-throwing `Create` API.
- Updated the pinned Windows Template Library dependency from 10.0 to 10.01.
- Added thin native wrappers for every specialized Microsoft Control Library
  family, including TreeView, ListView, Toolbar, DateTimePicker, MonthCalendar,
  Rebar, Pager, TaskDialog, Tooltip, ImageList, and Flat Scroll Bar integration.
- Added a populated Common Controls gallery, runtime creation coverage,
  independent public-header checks, and a front-page screenshot.
- Expanded the native controls set with GroupBox, RadioButton, ListBox, and
  Slider; the controls gallery and runtime test now instantiate all ten public
  wrappers.
- Added `WindowBase`, a concise inheritance path that hides WTL's repeated CRTP
  class name while preserving `Window<T>` for compile-time and message-map use.
- Raised the required language level to C++20 and added convention-based message
  handlers with typed event objects, while retaining compatibility with existing
  WTL message maps.
- Added move-only wrappers for native labels, buttons, text boxes, check boxes,
  combo boxes, and progress bars.
- Added the RAII `UiTimer` wrapper with `std::chrono` intervals and automatic
  cancellation.
- Added `RunApplication` for a one-line, ABI-safe application entry point.
- Converted every example to the macro-free API and added control galleries,
  bringing the executable example catalog to 22 programs.
- Added C++20 window-like overloads for controls and timers plus direct
  `CommandEvent::IsClicked(control)` matching to remove routine HWND and
  notification boilerplate.
- Added `Window<T>::Close()` as a result-preserving convenience for the common
  `WM_CLOSE` path.

## 0.2.0 - 2026-08-06

- Added explicit DIP geometry and per-window DPI conversion.
- Added configurable window class traits, native resources, styles, and bounds.
- Added default `WM_DPICHANGED` suggested-rectangle handling.
- Added opt-in wait-aware message pumping and lifetime-safe HWND wake tokens.
- Added optional STA/MTA COM initialization owned by `Application`.
- Added seven examples, C++20 compatibility coverage, stress/performance gates,
  system-message recipes, and a liney-style native host fixture.

The 0.1 `Application::Run<Window>(show_command)` source form is unchanged.
