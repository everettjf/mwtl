<p align="center">
  <img src="docs/images/mwtl-mark.svg" width="132" alt="mwtl geometric sun logo">
</p>

<h1 align="center">mwtl</h1>

<p align="center">
  A bright, lightweight C++20 foundation for native Windows desktop applications.
</p>

<p align="center">
  <a href="https://github.com/everettjf/mwtl/actions/workflows/ci.yml"><img src="https://github.com/everettjf/mwtl/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="https://github.com/everettjf/mwtl/blob/main/LICENSE"><img src="https://img.shields.io/badge/license-MIT-17a589.svg" alt="MIT license"></a>
  <img src="https://img.shields.io/badge/C%2B%2B-20-146c94.svg" alt="C++20">
  <img src="https://img.shields.io/badge/platform-Windows-ff9f43.svg" alt="Windows">
</p>

<p align="center">
  <a href="#build-and-run">Build</a> ·
  <a href="#example-gallery">Examples</a> ·
  <a href="https://everettjf.github.io/mwtl/">Documentation</a> ·
  <a href="docs/api-0.2.md">API &amp; ownership</a>
</p>

`mwtl` is a lightweight, Windows-only modern C++ foundation built on ATL/WTL and Microsoft WIL. It keeps native HWND and Win32 message interoperability while reducing application/bootstrap boilerplate and making lifetime and error boundaries explicit.

The project is not a replacement for Qt, WinUI, XAML/QML, or a cross-platform/full-visual UI framework. It favors native Windows controls and does not hide HWNDs.

## Current status

Version 0.2 extends the milestone-1 foundation with:

- macro-free C++20 convention handlers discovered with `requires`;
- typed keyboard, mouse, resize, DPI, command, timer, paint, min/max, and raw-message events;
- RAII `UiTimer` with `std::chrono` intervals;
- native Label, Button, TextBox, CheckBox, ComboBox, and ProgressBar wrappers;
- one-line `RunApplication<MainWindow>()` process startup;
- DIP geometry conversion and per-window `DpiContext`;
- configurable class traits, styles, icons/cursor/background, and initial bounds;
- automatic `WM_DPICHANGED` suggested-rectangle handling (opt-out is explicit);
- a `MsgWaitForMultipleObjectsEx` pump preserving WTL message filters;
- a lifetime-safe, non-owning `WindowWakeup` token for worker-to-UI notification;
- optional application-owned STA/MTA COM initialization;
- a C++20 public API using concepts, `std::span`, three-way comparison,
  designated initializers, and `std::jthread` in the focused examples;
- independent C++20 consumer and public-header compile checks.

The original foundation provides:

- a CMake static library target named `mwtl::mwtl`;
- pinned official WTL and WIL dependencies;
- `mwtl::Application` with WTL `CAppModule` and message-loop lifetime management;
- a CRTP `mwtl::Window<T>` main-window base with native HWND access;
- exception containment around real WTL window-message dispatch and `wWinMain`;
- Unicode-only compilation and a Per-Monitor DPI Awareness V2 manifest for the hello executable;
- an interactive native-control `examples/hello` quick start;
- focused `Application` and `Window<T>` component demos under `examples/`;
- CTest coverage for normal exit, creation failure cleanup, message-handler exceptions, public-header independence, and the embedded DPI manifest;
- a static component documentation site under `site/`, ready for GitHub Pages deployment.

There is currently no layout system, declarative control DSL, theme/font system, general task dispatcher, Mica/backdrop, Direct2D, DirectWrite, terminal, or ConPTY abstraction. The control wrappers deliberately remain thin owners of real system HWNDs.

The [`v0.2.0` development plan](docs/mwtl-0.2-plan.md) defines the
advanced native window host: configurable HWND creation, per-window DPI,
wait-aware message pumping, thread-safe wake-up, system lifecycle
interoperability, and a liney-win migration spike. It does not move terminal,
rendering, or workspace product logic into mwtl.

Implementation details are recorded in the [0.2 design](docs/mwtl-0.2-design.md),
the [API and ownership reference](docs/api-0.2.md),
the [liney-win migration spike](docs/liney-win-migration-spike.md), and the
[native system-message recipes](docs/system-message-recipes.md). See also the
[0.2.0 release notes](docs/release-notes-0.2.0.md).
Performance results and the remaining manual gates are in the
[0.2 evidence](docs/performance-0.2.0.md) and
[release checklist](docs/release-checklist-0.2.0.md).

## Requirements

- Windows 10 version 1809 or newer, or Windows 11;
- x64 or ARM64;
- Visual Studio 2022 or newer with the MSVC C++ desktop tools, Windows SDK, and C++ ATL component;
- CMake 3.21 or newer;
- C++20 (`/std:c++20`) is the required and publicly propagated language standard.

Milestone 1 intentionally fails during CMake configuration on non-Windows, non-MSVC, and 32-bit targets. MinGW and clang-cl are not supported.

## Dependencies

The default configuration fetches immutable commits from the official WTL SourceForge repository and Microsoft WIL GitHub repository. Exact tags, commits, and licenses are recorded in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

For offline builds, provide both checked-out source trees:

```powershell
cmake -S . -B build/x64 `
  -G "Visual Studio 17 2022" -A x64 `
  -DMWTL_WTL_SOURCE_DIR=C:/deps/wtl `
  -DMWTL_WIL_SOURCE_DIR=C:/deps/wil
```

An embedding project may instead define `WTL::WTL` and `WIL::WIL` targets before calling `add_subdirectory(mwtl)`. If either target is already present, mwtl uses it and does not fetch that dependency. It never searches silently for an unidentified system copy.

## Build and run

The checked-in presets provide the shortest path when dependencies can be fetched:

```powershell
cmake --preset vs2026-x64
cmake --build --preset x64-debug
ctest --preset x64-debug
```

From a Visual Studio 2026 Developer PowerShell, configure once and build Debug x64:

```powershell
cmake -S . -B build/x64 -G "Visual Studio 18 2026" -A x64 `
  -DMWTL_BUILD_EXAMPLES=ON -DMWTL_BUILD_TESTS=ON
cmake --build build/x64 --config Debug --verbose
ctest --test-dir build/x64 -C Debug --output-on-failure
./build/x64/examples/hello/Debug/mwtl_hello.exe
```

Build Release x64:

```powershell
cmake --build build/x64 --config Release --verbose
ctest --test-dir build/x64 -C Release --output-on-failure
./build/x64/examples/hello/Release/mwtl_hello.exe
```

On a machine with the MSVC ARM64 compiler, ARM64 ATL libraries, and ARM64 Windows SDK libraries installed:

```powershell
cmake -S . -B build/arm64 -G "Visual Studio 18 2026" -A ARM64
cmake --build build/arm64 --config Debug --verbose
cmake --build build/arm64 --config Release --verbose
```

When mwtl is included with `add_subdirectory`, examples and tests default to off. Set `MWTL_BUILD_EXAMPLES=ON` or `MWTL_BUILD_TESTS=ON` explicitly when the parent wants them.

## Component examples

The repository contains **20 independently buildable GUI examples**. They are all managed by [examples/CMakeLists.txt](examples/CMakeLists.txt) and share the same Unicode, C++20, warning, and Per-Monitor V2 manifest setup. Every name and screenshot below links directly to its complete `main.cpp`.

| Component | Directory | CMake target | Demonstrates |
|---|---|---|---|
| Quick start | [`examples/hello`](examples/hello/main.cpp) | `mwtl_hello` | Smallest complete program |
| `mwtl::Application` | [`examples/application`](examples/application/main.cpp) | `mwtl_application_demo` | HINSTANCE observation, Run, exit code, and wWinMain boundary |
| `mwtl::Window<T>` | [`examples/window`](examples/window/main.cpp) | `mwtl_window_demo` | HWND access, typed convention handlers, WM_APP messages, and `Close()` |
| Native message | [`examples/native_message`](examples/native_message/main.cpp) | `mwtl_native_message_demo` | Typed `WM_APP` constant, payload, `PostMessageW`, and `OnMessage` |
| Keyboard | [`examples/keyboard`](examples/keyboard/main.cpp) | `mwtl_keyboard_demo` | `WM_KEYDOWN`, virtual-key values, and Escape-to-close |
| Mouse | [`examples/mouse`](examples/mouse/main.cpp) | `mwtl_mouse_demo` | Client coordinates from `WM_MOUSEMOVE` and `WM_LBUTTONDOWN` |
| Resize | [`examples/resize`](examples/resize/main.cpp) | `mwtl_resize_demo` | Width, height, and state from `WM_SIZE` |
| Timer | [`examples/timer`](examples/timer/main.cpp) | `mwtl_timer_demo` | `UiTimer`, `std::chrono`, typed timer IDs, and RAII cleanup |
| Native paint | [`examples/paint`](examples/paint/main.cpp) | `mwtl_paint_demo` | Direct HWND/GDI interoperability through `WM_PAINT` |
| Minimum size | [`examples/minmax`](examples/minmax/main.cpp) | `mwtl_minmax_demo` | A native minimum tracking size through `WM_GETMINMAXINFO` |
| Close policy | [`examples/close_policy`](examples/close_policy/main.cpp) | `mwtl_close_policy_demo` | Intercepting `WM_CLOSE` without changing `Application` lifetime policy |
| Window state | [`examples/window_state`](examples/window_state/main.cpp) | `mwtl_window_state_demo` | Restored, minimized, and maximized state from `WM_SIZE` |
| DPI | [`examples/dpi`](examples/dpi/main.cpp) | `mwtl_dpi_demo` | Per-window DPI and `WM_DPICHANGED` |
| Window options | [`examples/window_options`](examples/window_options/main.cpp) | `mwtl_window_options_demo` | Class traits, styles, DIP client size and centering |
| Wait-aware pump | [`examples/wait_aware`](examples/wait_aware/main.cpp) | `mwtl_wait_aware_demo` | Non-busy message/timer waiting |
| Safe wake-up | [`examples/wakeup`](examples/wakeup/main.cpp) | `mwtl_wakeup_demo` | Worker-to-HWND lifetime-safe wake notification |
| COM STA | [`examples/com_sta`](examples/com_sta/main.cpp) | `mwtl_com_sta_demo` | Optional COM apartment lifecycle |
| Native controls | [`examples/controls`](examples/controls/main.cpp) | `mwtl_controls_demo` | Label, Button, TextBox, CheckBox, ComboBox, ProgressBar, commands, and timer |
| Self-drawn host | [`examples/self_drawn_host`](examples/self_drawn_host/main.cpp) | `mwtl_self_drawn_host_demo` | Dirty-frame wake-up, native GDI and wait-aware pumping |
| System lifecycle | [`examples/system_lifecycle`](examples/system_lifecycle/main.cpp) | `mwtl_system_lifecycle_demo` | Power/display/settings/IME/end-session/accessibility messages |

Build any one directly, for example:

```powershell
cmake --build build/x64 --config Debug --target mwtl_native_message_demo
./build/x64/examples/native_message/Debug/mwtl_native_message_demo.exe
```

See [examples/README.md](examples/README.md) for the complete target and run-command index.

## Visual quick start

The full catalog above includes infrastructure examples whose windows are
intentionally plain. This section instead highlights the examples where the
visible result helps explain the API. Code is on the left and its actual x64
Debug window is on the right.

<table>
  <tr>
    <th width="54%" align="left">Hello: one window, one label, one button</th>
    <th width="46%" align="left">Result</th>
  </tr>
  <tr>
    <td valign="top"><pre><code>class MainWindow final
    : public mwtl::Window&lt;MainWindow&gt; {
public:
    void BuildUI() {
        message_.Create(*this, {100}, L"Hello, mwtl", bounds);
        button_.Create(*this, {101}, L"Try it", button_bounds);
    }

    void OnCommand(const mwtl::CommandEvent&amp; event) {
        if (event.IsClicked(button_)) {
            message_.SetText(L"No message-map macros.");
        }
    }

private:
    mwtl::Label message_;
    mwtl::Button button_;
};</code></pre><a href="examples/hello/main.cpp">Open the complete Hello source →</a></td>
    <td valign="top"><a href="examples/hello/main.cpp"><img width="100%" src="docs/images/examples/hello.png" alt="Hello window with a native label and button"></a></td>
  </tr>
  <tr>
    <th align="left">Native controls: a complete small form</th>
    <th align="left">Result</th>
  </tr>
  <tr>
    <td valign="top"><pre><code>void BuildUI() {
    name_.Create(*this, {102}, L"mwtl developer", name_bounds);
    greet_.Create(*this, {103}, L"Say hello", button_bounds);
    enabled_.Create(*this, {104}, L"Button enabled", check_bounds);
    accent_.Create(*this, {105}, combo_bounds);
    progress_.Create(*this, {106}, progress_bounds);
    heartbeat_.Start(*this, {1}, 1s);
}

void OnCommand(const mwtl::CommandEvent&amp; event) {
    if (event.IsClicked(greet_)) {
        status_.SetText(L"Hello, " + name_.GetText());
    }
}</code></pre><a href="examples/controls/main.cpp">Open the complete controls source →</a></td>
    <td valign="top"><a href="examples/controls/main.cpp"><img width="100%" src="docs/images/examples/controls.png" alt="Native controls form with text box, buttons, combo box and progress bar"></a></td>
  </tr>
  <tr>
    <th align="left">Native painting: direct GDI remains available</th>
    <th align="left">Result</th>
  </tr>
  <tr>
    <td valign="top"><pre><code>void OnPaint(mwtl::PaintEvent&amp; event) {
    ::FillRect(event.GetDC(), &amp;client, background);
    ::DrawTextW(event.GetDC(), text, -1, &amp;client,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}</code></pre><a href="examples/paint/main.cpp">Open the complete painting source →</a></td>
    <td valign="top"><a href="examples/paint/main.cpp"><img width="100%" src="docs/images/examples/paint.png" alt="Native GDI paint example"></a></td>
  </tr>
</table>

## Tests

`MWTL_BUILD_TESTS` defaults on for a top-level build and off when mwtl is embedded. The test suite covers:

- successful main-window creation and normal loop exit;
- deterministic module-initialization and message-loop-registration failure cleanup;
- `BuildUI` exception causing Create failure and nonzero Run result;
- a WTL message-handler exception being contained by the real WindowProc boundary;
- exact `CAppModule`/message-loop cleanup counts on each path;
- independent C++20 compilation of every public header;
- macro-free convention dispatch and common native-control/timer lifecycle;
- extraction and inspection of the final hello EXE manifest with the Windows SDK manifest tool.
- DPI conversion edge cases, C++20 consumption, custom class traits, COM STA,
  wait-handle dispatch, callback exception containment, 2,000-message wake-up
  bursts, idle CPU behavior, and a liney-style native host fixture.

The repository CI workflow builds and tests x64 Debug and Release and separately cross-builds Debug and Release for ARM64.

## GitHub Pages

The documentation site source is in [site/](site/). It includes a landing page, build guide, and one usage page for every current public component. The pinned-action workflow [.github/workflows/pages.yml](.github/workflows/pages.yml) uploads that directory directly to GitHub Pages.

After the repository is public, open **Settings → Pages**, select **GitHub Actions** as the source, then run the **Deploy GitHub Pages** workflow or push a change under `site/` to `main`. No Jekyll or Node build is required.

## Minimal application

```cpp
#include <mwtl/mwtl.h>

class MainWindow final : public mwtl::Window<MainWindow> {
public:
    void BuildUI() { SetTitle(L"mwtl Demo"); }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    return mwtl::RunApplication<MainWindow>(instance, show_command);
}
```

`BuildUI` runs during `WM_CREATE`, after the HWND is attached. `RunApplication`
owns the `Application`, module, message loop, and ABI exception boundary. The
HWND remains directly available through `GetHwnd()`.

## Modern event handlers

Common events are discovered at compile time. No registration table, virtual
event interface, or message-map macro is needed:

```cpp
void OnKeyDown(const mwtl::KeyEvent& event);
void OnMouseMove(const mwtl::MouseEvent& event);
mwtl::EventResult OnResize(const mwtl::ResizeEvent& event);
void OnCommand(const mwtl::CommandEvent& event);
void OnTimer(mwtl::TimerId id);
void OnPaint(mwtl::PaintEvent& event);
mwtl::EventResult OnMessage(const mwtl::WindowMessage& message);
```

A `void` handler consumes its message. Return `EventResult::Propagate()` when
WTL/default processing should continue, or `EventResult::Handled(value)` when a
specific native result is required.

### Typed keyboard input

```cpp
class KeyboardWindow final : public mwtl::Window<KeyboardWindow> {
public:
    void BuildUI() { SetTitle(L"Press Escape to close"); }

    mwtl::EventResult OnKeyDown(const mwtl::KeyEvent& event) {
        if (event.virtual_key == VK_ESCAPE) {
            static_cast<void>(Close());
            return mwtl::EventResult::Handled();
        }
        return mwtl::EventResult::Propagate();
    }
};
```

### Common native controls

```cpp
using mwtl::operator""_dip;

void BuildUI() {
    name_.Create(*this, {100}, L"Ada",
                 {{24.0_dip, 24.0_dip}, {280.0_dip, 32.0_dip}});
    greet_.Create(*this, {101}, L"Greet",
                  {{320.0_dip, 24.0_dip}, {120.0_dip, 32.0_dip}});
}

void OnCommand(const mwtl::CommandEvent& event) {
    if (event.IsClicked(greet_)) {
        SetTitle(L"Hello, " + name_.GetText());
    }
}

mwtl::TextBox name_;
mwtl::Button greet_;
```

The wrappers own real child HWNDs. Label, Button, TextBox, CheckBox, ComboBox,
and ProgressBar expose `GetHwnd()` whenever direct Win32 access is useful.

### RAII timer with chrono

```cpp
using namespace std::chrono_literals;

void BuildUI() {
    if (!timer_.Start(*this, kRefreshTimer, 1s)) {
        throw std::runtime_error("timer start failed");
    }
}

void OnTimer(mwtl::TimerId id) {
    if (id == kRefreshTimer) {
        Refresh();
    }
}

static constexpr mwtl::TimerId kRefreshTimer{1};
mwtl::UiTimer timer_;  // KillTimer is automatic.
```

### Raw messages and WTL compatibility

```cpp
static constexpr UINT kRefresh = WM_APP + 1;

mwtl::EventResult OnMessage(const mwtl::WindowMessage& message) {
    if (message.id == kRefresh) {
        return mwtl::EventResult::Handled(42);
    }
    return mwtl::EventResult::Propagate();
}
```

Existing WTL `BEGIN_MSG_MAP` code remains source-compatible. A derived WTL map
can still chain to `mwtl::Window<T>`; the convention layer and the legacy map
both execute inside the same `SafeWindowProc` exception boundary.

## License

mwtl is licensed under the MIT License. See [LICENSE](LICENSE). Third-party dependencies retain their own licenses and do not imply Microsoft or upstream endorsement.
