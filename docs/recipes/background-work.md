# Run background work safely

Get a `WindowWakeup` from the window on the UI thread, then copy it into a
`std::jthread`. The worker performs non-UI work and calls `TryWake()`. Override
`OnWakeup()` to consume results and update controls on the UI thread.

The minimal compiled implementation is `examples/wakeup/main.cpp`; the
full lifecycle model is demonstrated by `examples/hot_corners`.

Make the `std::jthread` a window member so destruction requests stop and joins.
Keep shared result data synchronized. `TryWake()` may return false during
shutdown; treat that as normal cancellation, not as permission to touch the UI
from the worker.

