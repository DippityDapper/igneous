# Code Reference

This document describes how Igneous source code is formatted, organized, and architected.

## Language and Build

- **Language:** C++20 (`CMAKE_CXX_STANDARD 20`, required)
- **Build system:** CMake 3.30+
- **Primary target:** Static library `igneous` (`igneous::igneous`), consumed via `cmake/Igneous.cmake` — see [Linking Igneous](linking.md)

## Directory Layout

```
include/igneous/     Public headers, grouped by subsystem
src/                 Implementation (.cpp) files, mirroring header layout
cmake/               CMake helpers for downstream projects
libs/                Third-party dependencies (SDL3, ENet, ImGui, etc.)
docs/                Project documentation (this tree)
tests/               Unit tests (see docs/tests.md)
```

Headers live under `include/igneous/<module>/`. Each module maps to a functional area:

| Module | Responsibility |
|--------|----------------|
| `engine/` | Core loop, math, utilities, timing |
| `input/` | Keyboard, mouse, gamepad, input layers |
| `rendering/` | Window and SDL renderer / ImGui integration |
| `scenes/` | Scene lifecycle and scene graph management |
| `resources/` | Sprites, animations, textures, audio |
| `networking/` | Identity, serialization, transport backends |

Implementation files in `src/` use the same module names. One header typically pairs with one `.cpp` when non-inline logic exists.

## Namespace and Includes

- Almost all engine types live in the `Engine` namespace.
- Exception: `ThreadSafeQueue` is a standalone template in the global namespace (`include/igneous/engine/ThreadSafeQueue.hpp`).
- Public includes use the `"igneous/..."` path from the `include/` root:

```cpp
#include "igneous/engine/Engine.hpp"
#include "igneous/scenes/Scene.hpp"
```

- Headers start with `#pragma once`.
- Forward declarations are preferred over heavy includes when only pointers or references are needed.

## Naming Conventions

| Element | Style | Examples |
|---------|-------|----------|
| Classes / structs | PascalCase | `Scene`, `ResourceManager`, `AnimationFrame` |
| Methods | PascalCase | `HandleEvents`, `GetScene`, `BufferAdd` |
| Public members | camelCase or descriptive names | `position`, `zoom`, `singleton` |
| Private members (networking) | Leading underscore | `_host`, `_isServer`, `_localId` |
| Static singleton state | `static inline` in header | `Input::layers`, `SceneManager::sceneRoot` |
| Template parameters | `T`, `Ret`, `Args`, or descriptive names | `Vec2<T>`, `Event<Ret, Args...>` |

## Class Design Patterns

### Engine entry point

Games subclass `Scene` and start the engine with:

```cpp
Engine engine;
return engine.Run<MyScene>("MainScene");
```

`Engine::Run` initializes subsystems, registers the initial scene, runs the main loop, and cleans up.

### Scene system

- `Scene` is the base class for all game states (menus, gameplay, UI).
- `SceneRoot` owns and updates active scenes.
- `SceneManager` holds the global `SceneRoot` singleton.
- Override virtual hooks (`Update`, `Render`, `HandleInputs`, `OnCreated`, `OnDestroyed`) rather than internal `*Internal` methods.

### Static managers

Several subsystems expose static interfaces rather than instances:

- `Input`, `Window`, `Renderer`, `ResourceManager`, `SceneManager`, `Time`

These are initialized by `Engine` and accessed globally during the game loop.

### Networking abstraction

- `NetworkInterface` defines the transport API (`SendToServer`, `SendToClient`, `Poll`, `Connected`).
- Implementations: `LocalNetwork` (in-process), `RemoteNetwork` (ENet), `SteamNetwork` (Steamworks, optional).
- `IdentityProvider` abstracts player identity; `LocalIdentity` and `SteamIdentity` are the concrete providers.
- `Serializer` / `Deserializer` handle binary message encoding.

### Resources and rendering

- `Sprite` references a `Vec2<float>` position and owns named `Animation` sequences.
- `ResourceManager` registers sprites by z-index, caches textures and audio, and drives sprite rendering each frame.
- `Renderer` queues draw calls; `Camera` transforms world space to screen space.

## Formatting

- **Indentation:** 4 spaces (no tabs)
- **Braces:** Opening brace on the same line for functions, classes, and control flow
- **Access specifiers:** `public:` / `private:` labels at reduced indent inside classes; members indented one level further
- **Spacing:** Space after keywords (`if`, `for`, `while`); space around binary operators
- **Line length:** No hard limit; prefer readable breaks in long parameter lists

Example class skeleton:

```cpp
namespace Engine
{
    class Example
    {
      private:
        int value = 0;

      public:
        void DoWork(double delta);
    };
}
```

## Documentation in Source

- Public APIs use Doxygen-style block comments (`/** @brief ... */`) above classes, methods, and members.
- Each header's first line links to its markdown reference: `// Doc: docs/classes/<Name>.md`
- Detailed usage and architecture notes live in `docs/`; headers carry concise API summaries.

## Optional Features

- **Steamworks:** Enabled with CMake option `IGNEOUS_STEAM`. Defines `IGNEOUS_STEAM_ENABLED` and activates `SteamNetwork` / `SteamIdentity`.
- **Dependencies:** SDL3, SDL3_image, SDL3_mixer, ENet, SQLiteCpp, ImGui, boost_pfr, miniupnpc

## Adding New Code

1. Place the header in the appropriate `include/igneous/<module>/` directory.
2. Add a matching `.cpp` in `src/<module>/` if implementation is needed (CMake globs `src/*.cpp` automatically).
3. Add `// Doc: docs/classes/<Name>.md` as the first line of the header.
4. Create or update the class doc in `docs/classes/`.
5. Add an entry to `docs/architecture.md`.
