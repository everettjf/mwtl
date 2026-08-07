# mwtl 0.3.0 release checklist

## Scope and metadata

- [x] Project, package, consumer requirement, and changelog use version 0.3.0.
- [x] Current supported architecture is explicitly x64-only.
- [x] WTL 10.01 and WIL revisions are immutable and documented.
- [x] Public API ownership and native-handle behavior are documented.
- [x] The example catalog contains all 22 executables and screenshots.

## Build and automated verification

- [x] x64 Debug configure, build, link, and all CTest cases pass locally.
- [x] x64 Release configure, build, link, and all CTest cases pass locally.
- [x] Every public header independently compiles under C++20.
- [x] Installed `find_package(mwtl 0.3 CONFIG REQUIRED)` consumer builds and runs.
- [x] CPack creates the expected x64 ZIP with headers, library, licenses, and config files.
- [x] Multi-monitor corner tests cover negative origins, mixed resolutions,
  seams, half-open bounds, tolerance, dwell, and one-shot firing.
- [x] GitHub x64 Debug and Release CI is green for the release commit.
- [x] Release workflow dry-run artifact is downloaded and inspected.

## Runtime verification

- [x] Hot Corners Release GUI creates, renders, and closes normally.
- [x] Menus, accelerator ownership, DPI font creation, placement persistence,
  notifications, and monitor refresh paths are covered by tests or the reference app.
- [x] File/folder dialog cancellation is runtime verified without user input.
- [x] Clipboard text round-trip is verified without destroying pre-existing data.
- [x] Synthetic `HDROP` parsing and accelerator dispatch tests pass.
- [x] Hot Corners test mode proves actions without injecting shell shortcuts.

## Publication

- [x] Changelog heading contains the release date.
- [ ] `v0.3.0` tag points at the validated release commit.
- [ ] GitHub Release contains the inspected x64 ZIP and generated notes.
