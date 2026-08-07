# Contributing

mwtl targets x64 Windows applications using C++20 and MSVC. Build and test both
Debug and Release before submitting a change. Public API changes need an
independent-header compile test, runtime coverage when behavior is observable,
and documentation of ownership, threading, and failure behavior.

The pre-1.0 API may change when that makes application code simpler or removes
an unsafe contract. Avoid compatibility shims unless a released production
consumer requires one.

```powershell
cmake --preset vs2026-x64
cmake --build --preset x64-debug
ctest --preset x64-debug
cmake --build --preset x64-release
ctest --preset x64-release
```

No exception may cross a Win32 callback. UI objects belong to their creating
thread; cross-thread work requires an explicitly documented handoff.
