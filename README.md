# mwtl

`mwtl` is a lightweight, Windows-only modern C++ foundation built on ATL/WTL and Microsoft WIL. It keeps native HWND and Win32 message interoperability while reducing application/bootstrap boilerplate and making lifetime and error boundaries explicit.

The project is not a replacement for Qt, WinUI, XAML/QML, or a cross-platform/full-visual UI framework. It favors native Windows controls and does not hide HWNDs.

## Current status

Milestone 1 provides:

- a CMake static library target named `mwtl::mwtl`;
- pinned official WTL and WIL dependencies;
- `mwtl::Application` with WTL `CAppModule` and message-loop lifetime management;
- a CRTP `mwtl::Window<T>` main-window base with native HWND access;
- exception containment around real WTL window-message dispatch and `wWinMain`;
- Unicode-only compilation and a Per-Monitor DPI Awareness V2 manifest for the hello executable;
- a blank, closable `examples/hello` quick start;
- focused `Application` and `Window<T>` component demos under `examples/`;
- CTest coverage for normal exit, creation failure cleanup, message-handler exceptions, public-header independence, and the embedded DPI manifest;
- a static component documentation site under `site/`, ready for GitHub Pages deployment.

There is currently no DIP/geometry API, DPI relayout, layout system, control DSL/wrappers, theme/font system, dispatcher, Mica/backdrop, Direct2D, or DirectWrite support.

## Requirements

- Windows 10 version 1809 or newer, or Windows 11;
- x64 or ARM64;
- Visual Studio 2022 with the MSVC C++ desktop tools, Windows SDK, and C++ ATL component;
- CMake 3.21 or newer;
- C++17 (`/std:c++17`); no C++20 feature is required.

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
cmake --preset vs2022-x64
cmake --build --preset x64-debug
ctest --preset x64-debug
```

From a Visual Studio Developer PowerShell, configure once and build Debug x64:

```powershell
cmake -S . -B build/x64 -G "Visual Studio 17 2022" -A x64 `
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
cmake -S . -B build/arm64 -G "Visual Studio 17 2022" -A ARM64
cmake --build build/arm64 --config Debug --verbose
cmake --build build/arm64 --config Release --verbose
```

When mwtl is included with `add_subdirectory`, examples and tests default to off. Set `MWTL_BUILD_EXAMPLES=ON` or `MWTL_BUILD_TESTS=ON` explicitly when the parent wants them.

## Component examples

All examples are managed by [examples/CMakeLists.txt](examples/CMakeLists.txt) and share the same Unicode, C++17, warning, and Per-Monitor V2 manifest setup.

| Component | Directory | CMake target | Demonstrates |
|---|---|---|---|
| Quick start | `examples/hello` | `mwtl_hello` | Smallest complete program |
| `mwtl::Application` | `examples/application` | `mwtl_application_demo` | HINSTANCE observation, Run, exit code, and wWinMain boundary |
| `mwtl::Window<T>` | `examples/window` | `mwtl_window_demo` | HWND access, WTL message maps, WM_APP messages, and direct WM_CLOSE |

See [examples/README.md](examples/README.md) for focused build commands. Each component directory contains its own source and usage notes.

## Tests

`MWTL_BUILD_TESTS` defaults on for a top-level build and off when mwtl is embedded. The test suite covers:

- successful main-window creation and normal loop exit;
- deterministic module-initialization and message-loop-registration failure cleanup;
- `BuildUI` exception causing Create failure and nonzero Run result;
- a WTL message-handler exception being contained by the real WindowProc boundary;
- exact `CAppModule`/message-loop cleanup counts on each path;
- independent C++17 compilation of every public header;
- extraction and inspection of the final hello EXE manifest with the Windows SDK manifest tool.

The repository CI workflow builds and tests x64 Debug and Release and separately cross-builds Debug and Release for ARM64.

## GitHub Pages

The documentation site source is in [site/](site/). It includes a landing page, build guide, and one usage page for every current public component. The pinned-action workflow [.github/workflows/pages.yml](.github/workflows/pages.yml) uploads that directory directly to GitHub Pages.

After the repository is public, open **Settings → Pages**, select **GitHub Actions** as the source, then run the **Deploy GitHub Pages** workflow or push a change under `site/` to `main`. No Jekyll or Node build is required.

## Minimal application

```cpp
#include <mwtl/mwtl.h>

class MainWindow final : public mwtl::Window<MainWindow> {
public:
    void BuildUI() {
        SetTitle(L"mwtl Demo");
    }
};

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    try {
        return mwtl::Application(instance).Run<MainWindow>(show_command);
    } catch (...) {
        return EXIT_FAILURE;
    }
}
```

`BuildUI` is called during `WM_CREATE`, after the HWND is attached. `SetTitle` returns `bool` so callers that need to react to failure can do so without relying on exceptions. The HWND remains directly available through `GetHwnd()`, including for `::SendMessageW(GetHwnd(), ...)`.

The main-window C++ object is stack-owned by `Application::Run`; Win32/WTL manages its HWND. Closing this milestone-1 main window exits the one message loop. Multi-main-window lifetime policy is intentionally deferred.

## License

mwtl is licensed under the MIT License. See [LICENSE](LICENSE). Third-party dependencies retain their own licenses and do not imply Microsoft or upstream endorsement.
