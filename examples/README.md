# mwtl examples

Every public milestone-1 component has a focused executable:

| Directory | Target | Focus |
|---|---|---|
| `hello` | `mwtl_hello` | Smallest complete mwtl program |
| `application` | `mwtl_application_demo` | `mwtl::Application`, process entry, run result, and instance observation |
| `window` | `mwtl_window_demo` | `mwtl::Window<T>`, HWND access, WTL message maps, and native messages |

Configure with `MWTL_BUILD_EXAMPLES=ON`, then build one target or all targets:

```powershell
cmake -S . -B build/x64 -G "Visual Studio 17 2022" -A x64 -DMWTL_BUILD_EXAMPLES=ON
cmake --build build/x64 --config Debug --target mwtl_application_demo
cmake --build build/x64 --config Debug --target mwtl_window_demo
```

With the repository presets, the equivalent full examples/test build is:

```powershell
cmake --preset vs2022-x64
cmake --build --preset x64-debug
```

All examples use the shared Per-Monitor V2 manifest in `example.manifest`.
