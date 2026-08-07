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

- [x] x64 Debug and Release build and all CTest cases pass locally and in CI.
- [x] MSVC native static analysis completes with no mwtl correctness findings.
- [x] Independent-header and installed-package consumers pass.
- [x] CPack ZIP contents and SHA-256 checksum are inspected.

## Runtime

- [x] The accepted 0.5.0 validation scope is the current Windows 11 build
  26200 x64 machine; its complete hidden-window suite passes.
- [x] Font changes and representative keyboard, IME, accessibility, settings,
  display, DPI, and power messages are covered by automated tests.
- [x] Windows 10, mixed-monitor, real assistive-technology, Remote Desktop,
  sleep/resume, and downstream soak validation are deferred beyond 0.5.0.

## Publication

- [x] README, API guide, changelog, security policy, and support scope agree.
- [x] The annotated release tag points to the fully validated commit.
- [x] GitHub Release contains the ZIP, checksum, dependency revisions, and
  generated release notes.
