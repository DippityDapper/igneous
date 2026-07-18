# Camera

**Header:** `include/igneous/engine/Camera.hpp`  
**Implementation:** `src/engine/Camera.cpp`  
**Namespace:** `Engine`

## Overview

World-space viewport camera with position, zoom, and optional boundary clamping. Supports screen-to-world mouse conversion.

## Key Members

| Member | Description |
|--------|-------------|
| `main` | Static pointer to the primary render camera |
| `position` | Camera center in world space (`Vec2<float>`) |
| `zoom` | Zoom level (clamped by `minZoom` / `maxZoom`) |
| `limitBounds` | When true, clamp position and zoom to limits |

## Virtual Hooks

Override `Update(double delta)` and `HandleInputs(InputLayer& layer)` for custom camera behavior. The engine calls `UpdateInternal` / `HandleEventsInternal` instead.

## Related

- [Vec2](Vec2.md), [InputLayer](InputLayer.md), [Renderer](Renderer.md)
