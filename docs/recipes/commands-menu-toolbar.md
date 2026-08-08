# Share commands across menus, toolbars, and shortcuts

Use one `CommandSet` as the action and state model. Add `Command` objects with
stable `ControlId` values, callbacks, and optional shortcuts. Populate a
`Toolbar`, build a `Menu`, create an `AcceleratorTable`, and return
`commands.Dispatch(event)` from the command handler after handling control-only
notifications.

The complete canonical implementation is `examples/commands/main.cpp`.

When enabled or checked state changes, update every native presentation and
redraw the menu bar. Do not duplicate action logic separately in the menu,
toolbar, and keyboard handlers.

