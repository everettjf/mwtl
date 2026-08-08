# Contributing

mwtl targets x64 and ARM64 Windows applications using C++20 with MSVC-compatible
toolchains. Build and test both Debug and Release before submitting a change.
Public API changes need an
independent-header compile test, runtime coverage when behavior is observable,
and documentation of ownership, threading, and failure behavior.

The 0.6 core follows [the stability policy](docs/stability.md). Breaking a stable
contract requires a deprecation period, migration note, and compatibility
fixture. Provisional APIs may receive source-compatible additions.

```powershell
cmake --preset vs2026-x64
cmake --build --preset x64-debug
ctest --preset x64-debug
cmake --build --preset x64-release
ctest --preset x64-release
```

CI additionally owns clang-cl, AddressSanitizer, and native ARM64 validation.
Contributors with an ARM64 toolset can use the `arm64-debug` presets locally.

No exception may cross a Win32 callback. UI objects belong to their creating
thread; cross-thread work requires an explicitly documented handoff.
