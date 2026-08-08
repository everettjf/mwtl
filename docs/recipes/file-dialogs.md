# Use file and folder dialogs

Create `FileDialogOptions` with the owner HWND, title, filters, and optional
default extension. Call `ShowOpenFileDialog`, `ShowSaveFileDialog`, or
`ShowFolderDialog`. Inspect `FileDialogResult::accepted`; distinguish
`Cancelled()` from failure.

The complete canonical implementation is
`examples/desktop_integration/main.cpp`.

Use an STA application configuration when the selected desktop feature requires
COM STA behavior. Keep dialogs parented to the application window.

