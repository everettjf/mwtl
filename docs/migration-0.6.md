# Migrating from 0.5 to 0.6

0.6 preserves the documented 0.5 source API. Rebuild library and application
together because pre-1.0 ABI compatibility is not guaranteed.

Checked operations now throw `mwtl::Error`, derived from `std::system_error`, so
existing handlers remain valid. Prefer `MustInvoke` when a Win32 call reports
through `GetLastError`; use `Must(success, error, operation)` when the error was
captured separately.

The provisional `Command` and `CommandSet` types share an application action's
ID, state, text, and handler. Existing `OnCommand` code need not migrate and may
delegate to `CommandSet::Dispatch` incrementally.

Installed consumers may set `MWTL_DEPENDENCY_MODE` to `AUTO`, `FETCH`, or
`SYSTEM`. Explicit WTL and WIL source directories still take priority.
