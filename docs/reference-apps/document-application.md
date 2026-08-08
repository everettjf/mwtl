# Document-style commands and desktop integration

Canonical sources: `examples/commands/main.cpp` and
`examples/desktop_integration/main.cpp`.

Together these demonstrate the conventional native document-window surface:
one command model shared by menu, toolbar, and keyboard shortcuts; editable
content and dirty state; open/save/folder dialogs; clipboard; file drops; task
dialogs; and persistent window placement.

## Composition

1. Give every action one stable `ControlId` and one callback.
2. Route command events through `CommandSet` after handling control
   notifications.
3. Synchronize enabled/checked state to each native presentation.
4. Parent desktop dialogs to the application HWND.
5. Treat cancellation separately from failure.
6. Capture placement in `OnClose()` and then propagate normal closing.

Use this pair when generating editors, viewers, small IDE-like tools, and
document utilities. Replace demo save callbacks with an explicit document model
and persistence layer; do not put file I/O directly into every UI surface.

