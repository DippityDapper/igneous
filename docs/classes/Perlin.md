# Perlin

**Header:** `include/igneous/engine/PerlinNoise.hpp`  
**Namespace:** `Engine`

## Overview

Classic 2D/3D Perlin noise with fBm (fractional Brownian motion), PNG export, and `.pnoise` parameter files.

## Parameters

| Member | Default | Description |
|--------|---------|-------------|
| `octaves` | 6 | fBm layer count |
| `scale` | 0.005 | Frequency scale |
| `lacunarity` | 2.0 | Frequency multiplier per octave |
| `gain` | 0.5 | Amplitude multiplier per octave |
| `width`, `height` | 512 | Surface dimensions for export |

## Sampling

- `Noise2`, `Noise3` — single octave, raw coordinates
- `Fbm2`, `Fbm3` — multi-octave using member params
- `Fbm2_01`, `Fbm3_01` — remapped to [0, 1]

## Persistence

- `SavePng(path)` — render noise to PNG
- `SaveData(path)` / `LoadData(path)` — human-readable `.pnoise` config
