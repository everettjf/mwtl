# Handle DPI and appearance

Express layout in DIPs and use `GetDpiContext()` for native measurements. Apply
DPI-dependent fonts during setup and refresh them from `OnDpiChanged()`, then
return `Propagate()` so the window completes normal DPI processing.

Use `ApplyWindowAppearance` as a best-effort request. High Contrast and older
Windows versions can override or reject composition choices. Provide accessible
names for controls whose visible label is insufficient.

Canonical implementations: `examples/dpi/main.cpp` and
`examples/appearance/main.cpp`.

