# mwtl 0.2.0 release notes

mwtl 0.2.0 adds the advanced native-window-host capabilities needed by a
self-drawn application shell while preserving source compatibility with 0.1.

Highlights:

- DIP geometry and per-HWND DPI conversion;
- configurable WTL window class traits and run-time window options;
- default Per-Monitor V2 suggested-rectangle handling;
- pluggable default or wait-aware message pumps;
- lifetime-safe cross-thread window wake-up;
- optional STA/MTA COM ownership;
- focused examples and C++20 public-header/consumer compatibility tests;
- a liney-win host compatibility fixture and migration guide.
- direct runtime coverage for native lifecycle, IME, accessibility-provider,
  end-session, wait-failure, and continuous-producer input paths.

There are no layout, control, theme, dispatcher, Direct2D, DirectWrite, Mica,
terminal, or ConPTY abstractions in this release.
