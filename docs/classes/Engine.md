# Engine

**Header:** `include/igneous/engine/Engine.hpp`  
**Implementation:** `src/engine/Engine.cpp`  
**Namespace:** `Engine`

## Overview

Main game engine class. Manages SDL initialization, the core game loop, input dispatch, scene management, and shutdown.

## Public API

| Method | Description |
|--------|-------------|
| `Run<T>(initialSceneName)` | Initialize, add scene of type `T`, run loop, clean up. Returns exit code. |
| `Quit()` | Set running flag false to exit the main loop. |

## Lifecycle

1. `Init()` — SDL, ENet, Input, SceneManager
2. `Update()` — event loop, scene update/render until quit
3. `Clean()` — resource teardown

## Usage

```cpp
Engine engine;
return engine.Run<GameScene>("Game");
```

## Related

- [SceneManager](SceneManager.md), [Scene](Scene.md), [Input](Input.md), [Renderer](Renderer.md)
