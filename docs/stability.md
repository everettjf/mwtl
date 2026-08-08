# API stability policy

mwtl 0.6 is a pre-1.0 release, but its core API is treated as stable. Minor
0.6.x releases preserve source compatibility for documented core APIs. ABI
compatibility is not promised: applications should rebuild mwtl and their code
with the same MSVC toolset and runtime configuration.

## Stability levels

- **Stable:** application lifetime, windows, typed events, native control
  ownership, DIP geometry, retained layout, message pumping, wakeups, checked
  operations, packaging, and raw Win32 escape hatches.
- **Provisional:** the 0.6 command model and optional appearance helpers.
- **Example:** example application code is not library API.
- **Internal:** `mwtl::detail`, `src/detail`, and test-only definitions.

Public removals or signature changes require a migration entry and compile-time
compatibility fixture. Deprecations remain for at least one minor release. The
project does not silently change ownership, thread-affinity, exception, or
failure semantics.

`tests/consumer_05_compat.cpp` is the executable source-compatibility contract
for representative 0.5 application forms and is compiled by every toolchain.

The maintained 0.6 configurations are 64-bit Windows 10 1809 or newer on x64
and ARM64 using MSVC, plus clang-cl on x64. Each is enforced in CI.
