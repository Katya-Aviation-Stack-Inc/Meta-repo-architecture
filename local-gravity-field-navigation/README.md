# Local Gravity Field Navigation (LGFN)

This module implements navigation using Earth's gravity field fluctuations, providing GPS-independent positioning with compact gravimeters and DSP filtering.

## Features

- Compact gravimeter integration.
- DSP filtering for gravity anomaly mapping.
- Absolute navigation without GPS, INS, or radio signals.
- Used for autonomous aviation.

## Structure

- `src/`: C++ core for filtering and navigation.
- `docs/`: Documentation and gravity mapping details.

## Building

Use CMake to build the module. Refer to the top-level README for general instructions.
