# mwtl 0.5.0 release checklist

## Correctness

- [x] Dynamic intrinsic measurement is runtime-tested for all 27 controls.
- [x] Every public `noexcept` declaration is audited for allocation and
  consumer callbacks.
- [x] Debug UI-thread ownership diagnostics cover controls and layout,
  including worker-thread rejection and moved-control ownership.
- [x] Callback, creation, destruction, and allocation failures do not cross
  Win32 ABI boundaries.

## Build and analysis

- [ ] x64 Debug and Release build and all CTest cases pass locally and in CI
  (local complete; CI pending).
- [x] MSVC native static analysis completes with no mwtl correctness findings.
- [x] Independent-header and installed-package consumers pass.
- [x] CPack ZIP contents and SHA-256 checksum are inspected.

## Runtime

- [ ] Windows 10 minimum-version and Windows 11 smoke tests pass (Windows 11
  build 26200 automated hidden-window suite complete; Windows 10 pending).
- [ ] Mixed-DPI, font, high contrast, keyboard, IME, accessibility,
  sleep/resume, and Remote Desktop scenarios are exercised (font changes and
  representative keyboard, IME, accessibility, settings, display, and power
  messages are automated; mixed-monitor DPI, real assistive technology,
  sleep/resume, and Remote Desktop remain manual).
- [ ] A downstream application completes a sustained-use soak test.

## Publication

- [ ] README, API guide, changelog, security policy, and support scope agree.
- [ ] The signed release tag points to the fully validated commit.
- [ ] GitHub Release contains the ZIP, checksum, dependency revisions, and
  generated release notes.
