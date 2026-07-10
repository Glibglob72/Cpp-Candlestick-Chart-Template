# OHLCV Viewer

[![CI](https://github.com/Glibglob72/Cpp-Candlestick-Chart-Template/actions/workflows/ci.yml/badge.svg)](https://github.com/Glibglob72/Cpp-Candlestick-Chart-Template/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A native Windows candlestick charting application written from scratch in C++17 — no UI framework, no charting library. Renders millions of 1-minute OHLCV bars at interactive framerates using a custom instanced OpenGL renderer.

## Features

- Custom OpenGL 3.3 instanced renderer: per-bar OHLC data is uploaded as instance attributes and the price-to-pixel transform runs in the vertex shader
- Visible-range culling with instance-buffer caching — only on-screen bars are re-uploaded
- Automatic fallback to a GDI renderer when OpenGL is unavailable
- Raw Win32 UI: 2D panning (left-drag), cursor-anchored zoom (right-drag), crosshair with OHLCV info box
- Trading-session grouping with optional 18:00 session lines
- CSV loader with progress reporting; malformed lines are counted and surfaced in the title bar

## Building

Requires Visual Studio 2022 and CMake 3.24+.

```
cmake -B build
cmake --build build --config Release
```

The executable is written to `build/Release/OHLCVViewer.exe`.

## Sample data

The build generates `data/sample_ohlcv_1m.csv` next to the executable — ten trading days of synthetic 1-minute futures bars (random-walk prices, CME-style 18:00–16:59 sessions, no weekends) — so you can run the app immediately without downloading anything.

To generate more or different data, use the bundled tool:

```
build\Release\generate_sample_data.exe <output.csv> [tradingDays] [seed]
```

## Real market data

For real historical futures data (e.g. CME Micro E-mini S&P 500 `MES` or Micro E-mini Nasdaq-100 `MNQ`), get OHLCV 1-minute bars from [Databento](https://databento.com). Export as CSV in the format below and drop the file into the `data` folder next to the executable — every `.csv` there appears in the app's file dropdown.

Note: market data is licensed by its vendor; don't redistribute purchased data.

## Data format

CSV with header, one bar per line:

```
date,time,open,high,low,close,volume
2024-01-15,09:30:00,4800.25,4805.50,4799.00,4803.75,1250
```

## Tests

Unit tests use GoogleTest (fetched automatically at configure time) and run via CTest:

```
ctest --test-dir build -C Release --output-on-failure
```

## License

[MIT](LICENSE)

Bundles the [glad](https://glad.dav1d.de/) OpenGL loader (public domain) and a Khronos platform header (MIT-style license, notice included in the file). Tests fetch [GoogleTest](https://github.com/google/googletest) (BSD-3-Clause) at configure time.

## AI assistance

Development of this project was assisted by Anthropic's Claude (Opus and Fable models), used for code review, unit tests, tooling, and CI setup.
