# Neuro-FCC (Flight Control Contour)

This module implements a biologically-inspired flight control system that learns pilot behavior, providing low-latency (<2ms) flight corrections and failure recovery.

## Features

- Learns pilot behavior as a motor reflex.
- Low-level C++ core for flight control.
- Corrects control during failures like a "light cerebellum".
- Receives trajectory from embedded AI model.

## Structure

- `src/`: C/C++ source code for the control contour.
- `docs/`: Documentation and design specifications.

## Building

Use CMake to build the module. Refer to the top-level README for general instructions.
