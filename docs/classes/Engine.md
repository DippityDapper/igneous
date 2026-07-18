# Engine

**Header:** `include/igneous/engine/Engine.hpp`  
**Implementation:** `src/engine/Engine.cpp`  
**Namespace:** `Engine`

## Overview

Main game engine class. Manages SDL initialization, the core game loop, input dispatch, scene management, and shutdown.

## Public API

| Method / Type | Description |
|---------------|-------------|
| `EngineInitSettings` | Optional window width, height, and title (defaults: 640×360, `"client"`) |
| `Run<T>(initialSceneName, settings = {})` | Initialize, add scene of type `T`, run loop, clean up. Returns exit code. |
| `Quit()` | Set running flag false to exit the main loop. |

## Lifecycle

1. `Init()` — SDL (using `EngineInitSettings`), ENet, Input, SceneManager
2. `Update()` — event loop, scene update/render until quit
3. `Clean()` — resource teardown; restores input baseline (`_default` layer), clears scene root and camera

## Usage

```cpp
Engine::EngineInitSettings settings;
settings.width = 1280;
settings.height = 720;
settings.title = "My Game";

Engine::Engine engine;
return engine.Run<GameScene>("Game", settings);
```

Default window (no settings argument):

```cpp
Engine::Engine engine;
return engine.Run<GameScene>("Game");
```

## Related

- [SceneManager](SceneManager.md), [Scene](Scene.md), [Input](Input.md), [Renderer](Renderer.md)
