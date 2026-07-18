# Animation

**Header:** `include/igneous/resources/Animation.hpp`  
**Implementation:** `src/resources/Animation.cpp`  
**Namespace:** `Engine`

## Overview

Timed sequence of `AnimationFrame` entries. Advances frame index based on FPS and elapsed delta.

## API

| Method | Description |
|--------|-------------|
| `SetFPS(fps)` | Frame rate |
| `IncrementElapsedTime(delta)` | Advance timer (called by ResourceManager) |
| `AddFrame(...)` | Multiple overloads: file, texture, atlas, grid |
| `GetCurrentFrame()`, `SetCurrentFrameIndex(i)` | Frame access |
| `GetFrames()` | All frames |
