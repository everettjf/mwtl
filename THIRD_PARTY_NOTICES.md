# Third-party notices

mwtl is an independent project. Microsoft and the upstream WTL and WIL projects do not endorse mwtl.

## Windows Template Library (WTL)

- Official repository: <https://git.code.sf.net/p/wtl/git>
- Release tag: `Release_10.0`
- Locked commit: `0e39a4d013fc56a854cb6f64dddee582db5601ab`
- License: Microsoft Public License (Ms-PL)
- Upstream license notice: each distributed WTL header contains the Ms-PL notice and links to <https://opensource.org/license/ms-pl-html>
- Acquisition: CMake `FetchContent`, or a caller-provided `WTL::WTL` target / `MWTL_WTL_SOURCE_DIR`

WTL is consumed from source and is not copied into this repository.

## Windows Implementation Library (WIL)

- Official repository: <https://github.com/microsoft/wil>
- Release tag: `v1.0.260126.7`
- Locked commit: `cbf677fb0a942557d08fd129f4c106a76247b2ec`
- License: MIT
- Upstream license: <https://github.com/microsoft/wil/blob/cbf677fb0a942557d08fd129f4c106a76247b2ec/LICENSE>
- Acquisition: CMake `FetchContent`, or a caller-provided `WIL::WIL` target / `MWTL_WIL_SOURCE_DIR`

WIL is consumed from source and is not copied into this repository.
