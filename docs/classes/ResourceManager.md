# ResourceManager

**Header:** `include/igneous/resources/ResourceManager.hpp`  
**Implementation:** `src/resources/ResourceManager.cpp`  
**Namespace:** `Engine`

## Overview

Static resource cache and sprite render pipeline. Manages textures, audio, mixer tracks, and z-sorted sprite drawing.

## Sprites

| Method | Description |
|--------|-------------|
| `RegisterSprite(sprite)` | Add to z-index render list |
| `UnregisterSprite(spriteId)` | Remove sprite |
| `UpdateSpriteZIndex(sprite, z)` | Re-sort |
| `RenderSprites(delta)` | Update animations and draw all sprites |

## Textures

| Method | Description |
|--------|-------------|
| `LoadTexture(filePath)` | Cached load; returns `shared_ptr<SDL_Texture>` |
| `CreateTexture(format, access, w, h)` | Create blank texture |
| `CleanExpired(maxPerCall)` | Purge expired weak cache entries |
| `SetScaleMode(mode)` | SDL texture scale mode |

## Audio

| Method | Description |
|--------|-------------|
| `LoadSound(filePath, properties)` | Cached audio stream |
| `GetMixer()` | SDL_mixer mixer instance |
| `Clean()` | Release all resources |

## Static Members

`mixer`, `trackCount` (16), `tracks` — audio playback state.
