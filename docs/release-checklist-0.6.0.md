# mwtl 0.6.0 release checklist

## Contract and metadata

- [ ] `project(mwtl VERSION 0.6.0)` matches tag `v0.6.0`.
- [ ] Changelog, migration guide, stability policy, README, and API guide agree.
- [ ] No stable 0.5 API removal is missing a compatibility fixture.
- [ ] Installed package contains licenses and all public/documentation files.

## Required validation

- [ ] MSVC x64 Debug and Release build and all tests pass.
- [ ] clang-cl x64 Release build and all tests pass.
- [ ] MSVC ARM64 Debug build and all tests pass on `windows-11-arm`.
- [ ] MSVC AddressSanitizer job passes.
- [ ] Native source coverage remains at or above 74%; archive the Cobertura
  report produced by CI.
- [ ] MSVC Native Recommended Rules analysis has no project-source findings
  (pinned WTL/WIL diagnostics are recorded separately).
- [ ] Independent headers, API surface, package consumer, examples, manifests,
  site links, resource lifetime, and layout quality gates pass.
- [ ] Release ZIPs for x64 and ARM64 are produced and their SHA-256 files match.

## Manual Windows checks

- [ ] Hot Corners reference app: persistence, tray commands, accelerators,
  multi-monitor refresh, fullscreen pause, and shutdown cleanup.
- [ ] Keyboard-only navigation, default/cancel behavior, Narrator naming, and
  100%, 200%, and 300% DPI.
- [ ] Light, dark, and high-contrast modes; unsupported backdrop fallback.
- [ ] Windows 10 1809 smoke test and current Windows 11 smoke test.

Release is blocked until every automated item and every applicable manual item
is checked with evidence linked from the release notes or workflow run.
