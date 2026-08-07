# Changelog

## 0.2.0 - 2026-08-06

- Added explicit DIP geometry and per-window DPI conversion.
- Added configurable window class traits, native resources, styles, and bounds.
- Added default `WM_DPICHANGED` suggested-rectangle handling.
- Added opt-in wait-aware message pumping and lifetime-safe HWND wake tokens.
- Added optional STA/MTA COM initialization owned by `Application`.
- Added seven examples, C++20 compatibility coverage, stress/performance gates,
  system-message recipes, and a liney-style native host fixture.

The 0.1 `Application::Run<Window>(show_command)` source form is unchanged.
