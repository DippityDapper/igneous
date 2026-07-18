# Scene

**Header:** `include/igneous/scenes/Scene.hpp`  
**Implementation:** `src/scenes/Scene.cpp`  
**Namespace:** `Engine`

## Overview

Base class for game states (menus, gameplay, UI). Managed by `SceneRoot`; lifecycle driven by internal methods that call virtual hooks.

## Members

| Member | Description |
|--------|-------------|
| `root` | Owning `SceneRoot` |
| `name`, `tag` | Identification and grouping |
| `singleton` | Always active; cannot be unloaded |

## Override These

| Hook | When |
|------|------|
| `OnCreated()` | Scene added |
| `Update(delta)` | Each frame |
| `Render()` | Each frame |
| `HandleInputs(layer)` | Input dispatch |
| `OnDestroyed()` | Scene removed |
| `OnActiveChanged(value)` | Active flag toggled |

## Related

- [SceneRoot](SceneRoot.md), [SceneManager](SceneManager.md)
