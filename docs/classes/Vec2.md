# Vec2

**Header:** `include/igneous/engine/Vec2.hpp`  
**Namespace:** `Engine`

## Overview

Generic 2D vector template (`Vec2<T>`) where `T` is an arithmetic type. Provides arithmetic operators, normalization, distance, dot/cross product, and projection.

## Common Instantiations

- `Vec2<float>` — positions, velocities, directions
- `Vec2<int>` — pixel coordinates, grid cells

## Notable Methods

| Method | Description |
|--------|-------------|
| `Normalized()` | Unit vector; returns (0,0) if zero length |
| `DistanceTo(to)` | Euclidean distance |
| `Dot`, `Cross`, `Proj`, `Comp` | Vector math |
| `Ceil()`, `Floor()` | Component-wise rounding to `Vec2<int>` |
