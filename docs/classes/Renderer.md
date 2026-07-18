# Renderer

**Header:** `include/igneous/rendering/Renderer.hpp`  
**Implementation:** `src/rendering/Renderer.cpp`  
**Namespace:** `Engine`

## Overview

Static SDL3 renderer and ImGui integration. Queues sprite/texture draws and presents each frame.

## Frame Pipeline

1. `BufferClear()` — clear screen, begin ImGui frame
2. `BufferAdd(...)` — queue world-space or screen-space draws (sprites support `rotation` in degrees)
3. `Render()` — ImGui draw + `SDL_RenderPresent`

## BufferAdd Overloads

| Signature | Space |
|-----------|-------|
| `(position, sprite, camera)` | World (camera transform) |
| `(position, texture, camera, centered?)` | World |
| `(position, sprite)` | Screen |
| `(position, texture, centered?)` | Screen |

## Related

- [Window](Window.md), [Camera](Camera.md), [Sprite](Sprite.md)
