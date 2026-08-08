# Compose responsive layout

Use `Column` for vertical flow, `Row` for horizontal flow, and `Overlay` for
layering. Size each child with `Auto()`, `Fixed(dip)`, or `Stretch(weight,
minimum, maximum)`. Use `_dip` literals for margins, gaps, and fixed sizes.

The canonical implementation is `examples/layout_gallery/main.cpp`.

Controls must already have valid HWNDs before `SetLayout`. Layout retains native
window identities, not ownership of the C++ wrapper. Combo boxes may require a
`native_size` option so their drop-down extent is larger than their layout cell.

