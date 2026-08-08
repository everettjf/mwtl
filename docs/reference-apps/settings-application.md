# Settings and validated forms

Canonical source: `examples/form_binding/main.cpp`.

This application combines native form controls, explicit model state,
`ValueBinding`, validation messages, focus recovery, responsive nested layout,
DPI-dependent fonts, accessible names, a default dialog button, and system
appearance.

## Composition

1. Controls and bindings are window members with compatible lifetimes.
2. `BuildUI()` creates every HWND before constructing the retained layout.
3. `PushModel()` initializes controls from explicit model values.
4. `PullModel()` validates the candidate before updating previews.
5. `OnCommand()` distinguishes commit actions from live control notifications.
6. `OnDpiChanged()` refreshes native fonts and propagates default processing.

Use this source as the base for preferences, account setup, properties, and
small data-entry applications. Add persistence behind the explicit model rather
than reading controls throughout the rest of the program.

