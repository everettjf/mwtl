# mwtl 0.2.0 release checklist

- [x] x64 Debug configure/build/link and 43 CTest cases pass locally.
- [x] x64 Release configure/build/link and 43 CTest cases pass locally.
- [x] C++17 library/public headers and C++20 consumer compile.
- [x] All public headers independently compile without a PCH.
- [x] All 19 example manifests are extracted from the EXE and checked for PMv2.
- [x] Seven 0.2 GUI examples created a visible HWND, accepted `WM_CLOSE`, and exited 0.
- [x] `add_subdirectory`, pre-provided target, and FetchContent consumers build.
- [x] DPI conversion, suggested rectangle, class/resource, COM, pump, wake,
  exception, cleanup and liney-style host fixtures pass.
- [x] Performance evidence recorded in `performance-0.2.0.md`.
- [x] README, Pages, design, recipes, upgrade notes and changelog synchronized.
- [ ] Actual multi-monitor 125/150/200% transitions (test host has one 96-DPI monitor).
- [ ] Full liney-win product migration/quality suite (Zig 0.15.2 is absent).
- [ ] ARM64 local build (explicitly excluded from this x64-focused run).

The unchecked product/environment gates prevent claiming a fully validated
release candidate; they do not indicate an x64 mwtl build or test failure.
