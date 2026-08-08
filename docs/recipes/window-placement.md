# Persist window placement

At startup, load a `SavedWindowPlacement` and call `RestoreWindowPlacement`.
During `OnClose()`, call `CaptureWindowPlacement` and persist it. Return
`Propagate()` so normal close processing continues.

The registry-backed canonical implementation is
`examples/desktop_integration/main.cpp`. Use an application-specific registry
key and do not persist raw transient HWND values.

