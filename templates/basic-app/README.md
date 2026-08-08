# mwtl basic application template

Copy this directory, then configure and build on Windows:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
```

For local mwtl development add `-DMWTL_SOURCE_DIR=C:/path/to/mwtl`.

