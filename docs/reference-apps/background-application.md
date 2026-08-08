# Background work and lifecycle

Canonical sources: `examples/hot_corners/main.cpp`,
`examples/hot_corners/hot_corner_model.cpp`, and `examples/wakeup/main.cpp`.

Hot Corners is the complete worker-oriented reference application. It combines
multi-monitor state, background polling, lifetime-safe wakeups, commands,
persistence, tray/desktop integration, accessibility-aware appearance, and DPI
behavior.

## Composition

1. Pure detection/state logic is separated from HWND code and has direct tests.
2. Workers never mutate controls.
3. `WindowWakeup` crosses the thread boundary without keeping the window alive.
4. The UI thread consumes shared results and changes native state.
5. Shutdown stops workers before dependent member state is destroyed.
6. A GUI self-test exercises startup and controlled termination.

Use this shape for sync clients, monitors, launchers, progress applications, and
utilities with tray or multi-monitor behavior.

