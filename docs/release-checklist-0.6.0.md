# mwtl 0.6.0 release checklist

## Contract and metadata

- [ ] `project(mwtl VERSION 0.6.0)` matches tag `v0.6.0`.
- [x] Changelog, migration guide, stability policy, README, and API guide agree.
- [x] No stable 0.5 API removal is missing a compatibility fixture.
- [x] Installed package contains licenses and all public/documentation files.

## Required validation

- [x] MSVC x64 Debug and Release build and all tests pass.
- [x] clang-cl x64 Release build and all non-GUI-launch tests pass; the GUI
  self-test is covered by the MSVC matrices because the hosted VS 2022 image
  cannot activate clang-cl GUI executables (Win32 14001).
- [x] MSVC ARM64 Debug build and all tests pass on `windows-11-arm`.
- [x] MSVC AddressSanitizer job passes.
- [x] Native source coverage remains at or above 74%; archive the Cobertura
  report produced by CI.
- [x] MSVC Native Recommended Rules analysis has no project-source findings
  (pinned WTL/WIL diagnostics are recorded separately).
- [x] Independent headers, API surface, package consumer, examples, manifests,
  site links, resource lifetime, and layout quality gates pass.
- [x] Release ZIPs for x64 and ARM64 are produced and their SHA-256 files match.

## Manual Windows checks

- [ ] Hot Corners reference app: persistence, tray commands, accelerators,
  multi-monitor refresh, fullscreen pause, and shutdown cleanup.
- [ ] Keyboard-only navigation, default/cancel behavior, Narrator naming, and
  100%, 200%, and 300% DPI.
- [ ] Light, dark, and high-contrast modes; unsupported backdrop fallback.
- [ ] Windows 10 1809 smoke test.
- [x] Current Windows 11 native GUI smoke test.

Release is blocked until every automated item and every applicable manual item
is checked with evidence linked from the release notes or workflow run.

## Evidence

- Automated build, test, analysis, sanitizer, architecture, and coverage gates:
  [CI run 31238085763](https://github.com/everettjf/mwtl/actions/runs/31238085763).
- Non-publishing x64/ARM64 package rehearsal:
  [Release package run 31238655129](https://github.com/everettjf/mwtl/actions/runs/31238655129).
  Both jobs built and tested the project, produced one ZIP and one checksum
  file, and uploaded their artifacts; the publish job was skipped.
- Downloaded artifact checksums were independently recomputed and matched:
  x64 `9543baecc2e983b87d07d682fbd0ed5d71f35df818fe19502a8fb4ec95dc0d86`;
  ARM64 `4b93a693a781b5282f1be3de3adca9f10448b986075460479272162e0c6885be`.
- Windows 11 Home build 26200 x64: the Release `mwtl.dpi`,
  `mwtl.appearance`, `mwtl.advanced.dpi_change`, and native
  `mwtl.hot_corners_self_test` processes all passed on 2026-08-07.
