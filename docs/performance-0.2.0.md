# mwtl 0.2.0 performance evidence

Measured 2026-08-06 on Windows 11 Home build 26200, Intel Core i9-14900F
(32 logical processors), one 96-DPI monitor, MSVC 19.40.33812.0 and Windows SDK
10.0.22621.0. Both revisions were Release x64 builds with the same WTL/WIL
source directories. The 0.1 baseline was commit
`f9c6bc0702be797cec983cd80dadfde90d72bf9d`.

## Hello startup

PowerShell started `mwtl_hello.exe`, waited for input-idle and a non-null main
window handle, recorded elapsed time, posted `WM_CLOSE`, and verified exit. Five
warm-ups preceded 30 samples.

| Revision | median | p95 | min | max |
|---|---:|---:|---:|---:|
| 0.1 baseline | 43.797 ms | 170.555 ms | 35.343 ms | 445.246 ms |
| 0.2 candidate | 36.409 ms | 43.961 ms | 33.186 ms | 53.012 ms |

The 0.2 median is 16.9% lower than this run's 0.1 median; it therefore passes
the “no more than 5% regression” gate. Process scheduling noise is visible in
the baseline tail, so the median is the release gate.

## Pump gates

`mwtl_advanced_lifecycle_test.exe wake_latency` performed 500 sequential
worker-to-UI registered-message round trips: median was below the microsecond
display resolution and p95 was 10 microseconds (gate: at most 16 ms).

`mwtl_advanced_lifecycle_test.exe wake_stress` delivered all 2,000 queued wake
messages without loss. `idle_efficiency` spent 310 ms in ten 20-ms wait cycles
and consumed 0.000 ms process CPU at FILETIME resolution (gate in the automated
test: no more than 100 ms process CPU). Both tests returned zero.

These measurements cover mwtl host and pump overhead, not liney-win terminal
throughput or renderer performance.
