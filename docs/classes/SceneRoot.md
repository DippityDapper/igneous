# SceneRoot

**Header:** `include/igneous/scenes/SceneRoot.hpp`  
**Implementation:** `src/scenes/SceneRoot.cpp`  
**Namespace:** `Engine`

## Overview

Container for all scenes. Handles create, update, render, input, load/unload, and deferred removal.

## Scene Management

| Method | Description |
|--------|-------------|
| `AddScene<T>(name, tag?, active?, singleton?)` | Create and register scene |
| `RemoveScene(name)` | Queue for removal |
| `RemoveScenes(tag)` | Queue all matching tag |
| `LoadScene(name, unloadAll?)` | Activate scene |
| `UnloadScene(name)` | Deactivate (not singletons) |
| `UnloadAllScenes()` | Deactivate all non-singletons |
| `GetScene(name)`, `SceneExists(name)` | Lookup |

## Per-Frame

`Update(delta)`, `Render()`, `HandleEvents(layer)`, `ProcessRemoveScenesQueue()`
