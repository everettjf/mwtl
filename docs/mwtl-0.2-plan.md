# mwtl 0.2 development plan

Status: proposed

Target release: `v0.2.0`

Theme: advanced native window host
Reference consumer: [liney-win](https://github.com/everettjf/liney-win)

## 1. Release objective

mwtl 0.2 will make the library suitable for a high-frequency, fully self-drawn
native application shell without turning mwtl into a terminal, rendering, or
workspace framework.

The release is successful when liney-win can use mwtl for its process-level WTL
integration, top-level HWND creation, exception-safe native message dispatch,
Per-Monitor DPI handling, and wait-aware message pumping while retaining its own
Direct2D/DirectWrite/D3D11 renderer, ConPTY sessions, terminal model, tabs,
splits, workspace, accessibility semantics, and product features.

The intended boundary is:

> mwtl owns reliable native application and window infrastructure; the consumer
> owns rendering and product behavior.

## 2. Compatibility commitments

- Windows 10 version 1809 and newer, and Windows 11.
- MSVC with Visual Studio 2022 or a later toolset explicitly verified by CI.
- x64 and ARM64.
- C++20 is the public minimum and default language standard.
- C++20 consumers such as liney-win can use concepts, spans, designated
  initializers, and standard stop tokens without an adapter layer.
- Unicode only.
- Per-Monitor DPI Awareness V2.
- WTL and WIL remain pinned to immutable official commits.
- `mwtl::mwtl` remains a normal static-library target.
- `add_subdirectory` and FetchContent integration remain supported.
- HWND, Win32 messages, WPARAM, LPARAM, and LRESULT remain directly accessible.
- No C++ exception may cross a Win32 callback, WTL thunk, message-pump callback,
  or process entry-point boundary.

0.2 may refine APIs introduced only in 0.1 because the project has not declared
ABI stability. Source compatibility should nevertheless be preserved wherever
it does not compromise lifetime or error handling.

## 3. Scope

### 3.1 Required capabilities

1. Configurable top-level window class and creation options.
2. DPI-independent geometry and a per-window DPI context.
3. Correct `WM_DPICHANGED` handling with the OS-suggested rectangle.
4. A standard message pump and an opt-in wait-aware pump.
5. Thread-safe wake-up without a general task/async framework.
6. Reliable paint, resize, display, power, settings, IME, cursor, and
   accessibility message interoperability.
7. Optional COM STA initialization owned by `Application`.
8. A migration spike proving that liney-win can consume the new infrastructure.
9. Examples and tests for every new public component.
10. Performance and lost-wakeup regression gates.

### 3.2 Explicit non-goals

The following do not belong in mwtl 0.2:

- Ghostty or another VT parser;
- ConPTY session or child-process management;
- terminal grids, scrollback, selection, find, or key encoding;
- a Direct2D, DirectWrite, D3D11, or swap-chain abstraction;
- a retained-mode control tree or general layout framework;
- terminal tabs, binary splits, workspaces, Git, SSH, or agent sessions;
- application configuration, updates, AI integration, diagnostics archives, or
  packaging;
- product-specific UI Automation providers;
- a thread pool, futures/promises, coroutines, cancellation, or a general
  dispatcher;
- Windows 11 visual effects as a hard dependency.

## 4. Proposed public API direction

All API sketches are provisional. Names may change during implementation, but
the ownership and failure semantics are requirements.

### 4.1 Geometry and DPI

```cpp
namespace mwtl {

struct Dip {
    float value = 0.0f;
};

struct PointDip {
    Dip x;
    Dip y;
};

struct SizeDip {
    Dip width;
    Dip height;
};

struct RectDip {
    PointDip origin;
    SizeDip size;
};

struct Thickness {
    Dip left;
    Dip top;
    Dip right;
    Dip bottom;
};

class DpiContext final {
public:
    static DpiContext FromWindow(HWND window) noexcept;

    UINT GetDpi() const noexcept;
    float GetScale() const noexcept;
    int ToPixels(Dip value) const noexcept;
    Dip FromPixels(int value) const noexcept;
};

}  // namespace mwtl
```

Requirements:

- geometry types are trivial value types with no HWND ownership;
- conversion and rounding are centralized and documented;
- invalid/null HWND input has deterministic behavior;
- no process-global mutable DPI state;
- arithmetic remains constexpr and C++20-compatible;
- the public API distinguishes DIP values from physical pixels.

### 4.2 Window configuration

```cpp
struct WindowClassOptions {
    const wchar_t* name = nullptr;
    UINT style = CS_HREDRAW | CS_VREDRAW;
    HICON icon = nullptr;       // non-owning
    HICON small_icon = nullptr; // non-owning
    HCURSOR cursor = nullptr;   // non-owning
    HBRUSH background = nullptr; // non-owning
};

struct WindowOptions {
    const wchar_t* title = L"mwtl application";
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD extended_style = 0;
    RectDip initial_bounds{};
    bool center_in_work_area = true;
};
```

Requirements:

- the consumer can request `CS_DBLCLKS`;
- resources are explicitly non-owning;
- initial client and outer-window sizing semantics are not mixed;
- size conversion uses the target monitor's DPI and
  `AdjustWindowRectExForDpi`;
- work-area clamping never makes the title bar unreachable;
- `CreateWindowExW` errors preserve `GetLastError()`;
- the 0.1 `Application::Run<MainWindow>(show_command)` form remains available
  through sensible defaults.

### 4.3 Native message access

Typed C++20 convention handlers are the primary dispatch mechanism. mwtl does
not replace WPARAM/LPARAM/LRESULT with a closed event model.

Every consumer handler must be reached through the existing exception-safe WTL
WindowProc boundary. 0.2 should add documented patterns for:

- typed convention handlers for common messages;
- `OnMessage(const WindowMessage&)` for uncommon or application-defined messages;
- optional legacy WTL maps, message ranges, alternate map IDs, and chaining;
- preserving propagation and the native LRESULT with `EventResult`;
- creation, normal dispatch, and destruction-stage failures.

### 4.4 Message-pump policies

```cpp
struct StandardMessagePump {
    int Run() noexcept;
};

struct WaitAwarePumpOptions {
    std::span<const HANDLE> handles{}; // non-owning
    DWORD idle_timeout_ms = INFINITE;
};
```

A wait-aware policy must support consumer callbacks equivalent to:

```cpp
void AfterDispatch(const MSG& message);
void OnIdle();
DWORD GetWaitTimeout() const noexcept;
```

Requirements:

- drain queued messages with `PeekMessageW`;
- preserve `TranslateMessage` and `DispatchMessageW` behavior;
- use `MsgWaitForMultipleObjectsEx` without lost-wakeup races;
- return the exact `WM_QUIT` exit code;
- avoid a busy loop when idle;
- allow render-on-dirty and bounded fallback ticks;
- contain exceptions from every consumer callback;
- clean up `_Module` and message-loop registration on all pump failures;
- never own caller-provided wait handles unless an owning type explicitly says
  so.

The standard WTL loop remains the default. The advanced pump is opt-in.

### 4.5 Window wake-up

```cpp
class WindowWakeup final {
public:
    bool TryWake() const noexcept;
};
```

Requirements:

- safe to call from any thread;
- implemented with a private registered or `WM_APP`-range message;
- holds no owning pointer to the C++ window object;
- returns false after `WM_NCDESTROY`;
- cannot invoke arbitrary callbacks on the producer thread;
- does not grow into a general dispatcher in 0.2.

### 4.6 System lifecycle hooks

0.2 must document and test direct handling for:

- `WM_PAINT` and `WM_SIZE`;
- `WM_DPICHANGED` and `WM_DISPLAYCHANGE`;
- `WM_SETTINGCHANGE` and high-contrast refresh;
- `WM_POWERBROADCAST` suspend/resume;
- `WM_GETMINMAXINFO`;
- `WM_IME_STARTCOMPOSITION` and `WM_IME_COMPOSITION`;
- `WM_SETCURSOR` and mouse-wheel coordinates;
- `WM_GETOBJECT` provider hand-off;
- `WM_QUERYENDSESSION` and `WM_ENDSESSION`.

mwtl may provide small helpers, but the raw messages and return values must
remain available. Rendering resource reconstruction, IME cursor location, and
accessibility semantics stay consumer-owned.

### 4.7 Optional COM initialization

```cpp
enum class ComApartment {
    none,
    sta,
    mta,
};

struct ApplicationOptions {
    ComApartment com_apartment = ComApartment::none;
};
```

Requirements:

- use WIL or equivalent RAII;
- retain and report the original HRESULT;
- diagnose `RPC_E_CHANGED_MODE` distinctly;
- initialize before main-window construction;
- uninitialize after the window and pump have stopped;
- do not change COM state unless explicitly requested.

## 5. Work packages

### WP0 — Baselines and compatibility fixtures

Deliverables:

- record 0.1 API and behavior baselines;
- add build-only C++20 consumer fixtures;
- capture current hello startup, idle CPU, and shutdown measurements;
- create a small self-drawn reference application that mimics liney-win's
  outer window without terminal code.

Exit gate:

- all 0.1 examples build without source changes;
- x64 Debug/Release and ARM64 compile/link CI are green;
- baseline measurements are committed and reproducible.

### WP1 — DPI value types and context

Deliverables:

- `Dip`, point/size/rect/thickness types;
- centralized conversion and rounding rules;
- `DpiContext`;
- OS-suggested `WM_DPICHANGED` rectangle handling;
- DPI transition example.

Exit gate:

- conversion tests cover 96, 120, 144, 168, 192, and 240 DPI;
- negative coordinates and multi-monitor work areas are covered;
- actual 100%, 125%, 150%, and 200% runs are recorded where the test host
  permits display configuration;
- no global DPI state exists.

### WP2 — Configurable native window host

Deliverables:

- class and creation options;
- custom icons, cursor, background, class styles, and window styles;
- DPI-aware initial client/outer sizing;
- centered and explicit-position modes;
- minimum-size example using native `WM_GETMINMAXINFO`.

Exit gate:

- a window using `CS_DBLCLKS`, a custom icon, and no background brush creates
  successfully;
- creation failures retain their Win32 error;
- `WM_NCDESTROY` invalidates all observers;
- old `Run(show_command)` usage still builds.

### WP3 — Wait-aware pump and wake-up

Deliverables:

- standard and wait-aware pump implementations;
- wait HANDLE support;
- idle timeout callback;
- `WindowWakeup`;
- render-on-dirty sample with no Direct2D dependency.

Exit gate:

- no lost wakeup in a sustained producer/consumer stress test;
- `WM_QUIT` exit codes are preserved;
- idle CPU stays below the documented threshold;
- normal input remains responsive during continuous background production;
- exceptions from `AfterDispatch`, `OnIdle`, and wait processing cannot cross
  the pump boundary;
- every initialized WTL/COM resource is released on injected failures.

### WP4 — System-message interoperability

Deliverables:

- documented lifecycle handler recipes;
- power/display/settings/IME/accessibility examples;
- optional COM apartment support;
- modal move/resize paint regression sample.

Exit gate:

- suspend/resume and display-change smoke paths do not retain stale HWND state;
- IME committed text still reaches `WM_CHAR`;
- a consumer-supplied UI Automation provider can be returned through
  `WM_GETOBJECT`;
- high-contrast notification reaches consumer code;
- no D2D/DWrite header appears in a public mwtl header.

### WP5 — liney-win migration spike

The spike should be developed in a liney-win branch after WP1–WP4 are stable.

Replace only:

- process/application bootstrap;
- manual window-class registration;
- `CreateWindowExW` setup;
- `GWLP_USERDATA` thunk storage;
- manual message-pump boilerplate;
- process DPI bootstrap and per-window DPI plumbing.

Keep unchanged:

- `IRenderer`/`D2DRenderer`;
- D3D11 glyph atlas and DirectWrite shaping;
- ConPTY and Ghostty VT integration;
- tabs, panes, workspace, SSH, agents, configuration, updates, and AI;
- liney-win's product-specific accessibility provider.

Exit gate:

- liney-win builds with mwtl through `add_subdirectory` or FetchContent;
- the existing unit, visual-regression, smoke, soak, accessibility, display,
  power-resume, and compatibility scripts retain their prior results;
- startup time, idle CPU, resize responsiveness, and terminal throughput show
  no material regression;
- manual `RegisterClassExW`, main-window `CreateWindowExW`, thunk, and primary
  pump code are removed from liney-win;
- the migration does not require a liney-specific public API in mwtl.

### WP6 — Documentation and release

Deliverables:

- API reference and ownership tables;
- at least one example per new public component;
- a self-drawn-host example and a wait-aware-pump example;
- upgrade notes from 0.1;
- updated Pages content;
- dependency and license audit;
- `v0.2.0` changelog and release checklist.

Exit gate:

- a new consumer can build each example from README commands;
- public headers independently compile under C++20;
- package/add_subdirectory/FetchContent consumer checks pass;
- all required CI and manual gates have recorded evidence.

## 6. Suggested implementation sequence

The work should land as small, independently reviewable changes:

1. Baseline fixtures and performance harness.
2. DIP types and rounding tests.
3. `DpiContext` and DPI-transition example.
4. Window class/creation options.
5. DPI-aware initial sizing and work-area placement.
6. Pump policy interface with the existing loop as default.
7. Wait-aware pump and deterministic wake tests.
8. `WindowWakeup` lifetime implementation.
9. System lifecycle examples and optional COM initialization.
10. Self-drawn host integration example.
11. liney-win migration spike.
12. Documentation, compatibility audit, and release candidate.

No change should combine DPI, message-pump, and liney-win migration work in one
large patch.

## 7. Verification matrix

| Area | Required verification |
|---|---|
| x64 | Debug and Release configure, compile, link, and CTest |
| ARM64 | Debug and Release configure, compile, and link |
| Language | Public library, headers, examples, and consumer fixture compile with `/std:c++20` |
| Headers | Every public header independently compiles without a PCH |
| DPI | Conversion unit tests plus actual monitor-transition smoke tests |
| Window creation | Default and custom class/style/icon/cursor/background cases |
| Pump | Standard loop, wait HANDLE, idle timeout, wake race, exact exit code |
| Failures | Module init, loop registration, window creation, pump callback, COM init |
| Exceptions | Creation, normal dispatch, destruction, idle callback, wait callback, `wWinMain` |
| Lifecycle | `WM_NCDESTROY` invalidation and no callbacks after native destruction |
| Rendering host | Repaint during modal resize and no idle busy-spin |
| System state | Power resume, display change, high contrast, end session |
| IME | Candidate positioning hook and committed UTF-16 delivery |
| Accessibility | Consumer provider hand-off through `WM_GETOBJECT` |
| Integration | FetchContent, `add_subdirectory`, and caller-provided dependency targets |
| Reference app | liney-win unit, visual, smoke, soak, accessibility, and compatibility gates |

“Static review” and “runtime verified” must remain separate result categories.
An x64 result does not imply ARM64 success, and simulated DPI conversion tests
do not replace an actual monitor-transition run.

## 8. Performance gates

0.2 must publish the measurement command, host details, and raw result for each
gate. Proposed initial thresholds relative to the 0.1 baseline/reference host:

- hello startup median: no more than 5% regression;
- idle standard-pump CPU: no measurable sustained busy loop;
- idle wait-aware-pump CPU: no more than 0.5% of one logical CPU on the test
  host after settling;
- wake-to-dispatch p95: no more than 16 ms under the standard stress profile;
- interactive resize: no black/unpainted client region attributable to the
  mwtl host;
- shutdown: no additional five-second-or-longer tail caused by pump cleanup.

Thresholds should be adjusted only from recorded evidence, not to make a failing
implementation pass.

## 9. Risks and decisions

### Custom pump versus WTL assumptions

WTL components may assume `CMessageLoop` registration even when a consumer uses
an advanced pump. `Application` must continue registering the loop adapter and
must document which WTL filters/idle handlers remain active.

### Window class customization and template identity

The 0.1 fixed class declaration is insufficient for applications needing
`CS_DBLCLKS`, custom icons, or a null background. The design must not create a
different registered class accidentally for every runtime option. Class
identity and registration failure behavior require explicit tests.

### Wake-up lifetime

A copied wake token must not keep a destroyed window alive or post to a reused
HWND. The implementation needs a generation/lifetime mechanism rather than a
bare HWND copied indefinitely.

### DPI timing

Initial size calculation may occur before the final monitor is known. Window
placement must choose a target monitor deterministically and use its DPI before
calculating the outer rectangle.

### Scope pressure from liney-win

The migration spike may reveal attractive product helpers. A helper enters mwtl
only if it is independently useful to native Windows applications and can be
implemented without importing terminal/workspace semantics.

## 10. Release definition of done

`v0.2.0` is ready only when all of the following are true:

- every WP0–WP6 exit gate is satisfied or explicitly removed from scope before
  release-candidate implementation begins;
- no required result is represented only by a code review when the matrix calls
  for runtime verification;
- x64 Debug and Release are green locally and in CI;
- ARM64 Debug and Release compile/link in CI;
- C++20 evidence is present in build logs;
- all public headers independently compile;
- the wait-aware pump passes stress and lost-wakeup tests;
- all injected failure paths demonstrate paired WTL/COM cleanup;
- the liney-win migration spike passes its existing quality gates;
- documentation clearly separates mwtl infrastructure from consumer rendering
  and product responsibilities;
- no Direct2D, terminal, layout framework, or general dispatcher dependency has
  entered the mwtl core;
- README, examples, design notes, third-party notices, changelog, and Pages are
  synchronized with the released API.
