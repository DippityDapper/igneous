# Sprite

**Header:** `include/igneous/resources/Sprite.hpp`  
**Implementation:** `src/resources/Sprite.cpp`  
**Namespace:** `Engine`

## Overview

Drawable animated or static graphic. References external position, owns named animations, registered with ResourceManager for z-ordered rendering.

## Key Members

| Member | Description |
|--------|-------------|
| `position` | Reference to `Vec2<float>` world position |
| `centered` | Draw from center vs top-left |
| `render` | Visibility toggle |
| `scaleX`, `scaleY` | Render scale |
| `rotation` | Clockwise rotation in degrees (`0` = upright) |
| `zIndex`, `id` | Draw order and manager ID |

## Animations

| Method | Description |
|--------|-------------|
| `AddAnimation(name, setCurrent?, keepIndex?)` | Add sequence |
| `SetCurrentAnimation(name, keepIndex?)` | Switch animation |
| `GetCurrentAnimation()`, `GetCurrentFrame()` | Active frame access |
| `SetTexture`, `SetAtlas`, `GetSourceRect` | Frame manipulation |
| `IsMouseWithin(camera?)` | Hit test in world space |

## Constructors

Support empty, file path, shared texture, and atlas/grid variants. Copy constructor duplicates state and re-registers.
