# Igneous

Igneous is a C++20 game engine built on SDL3. It provides a scene-based game loop, 2D rendering with ImGui, input handling, resource management (sprites, animation, audio), SQLite persistence, and optional multiplayer via ENet or Steamworks.

This repository builds the **engine library** (`libigneous.a` / `igneous.lib`). Games link against it as a CMake subproject.

## Features

- **Engine core** — SDL3 init, fixed-timestep delta, main loop
- **Scenes** — Stackable game states with load/unload and tags
- **Rendering** — SDL renderer, camera transforms, z-sorted sprites, ImGui
- **Input** — Keyboard, mouse, gamepad, priority input layers
- **Resources** — Texture/audio caching, sprite animations, atlas support
- **Networking** — Local simulation, ENet (LAN/WAN), optional Steam Networking Sockets
- **Utilities** — CFG files, SQLite wrapper, Perlin noise, thread pool

## Requirements

| Tool | Version |
|------|---------|
| CMake | 3.30+ |
| C++ compiler | C++20 (GCC 11+, Clang 14+, MSVC 2022+) |
| Git | For submodules |

Optional: [Ninja](https://ninja-build.org/) (faster builds), Steamworks SDK (for `IGNEOUS_STEAM`).

## Quick Start (engine development)

```bash
git clone --recurse-submodules <repo-url> igneous
cd igneous
cmake --preset debug
cmake --build --preset debug
```

This builds `libigneous.a`. Link a game against it (see "Using Igneous in a Game" below) to actually run something.

### Optional: Steamworks

```bash
cmake -B build -DIGNEOUS_STEAM=ON
```

Place the Steamworks SDK under `libs/steamworks/` first. The directory is gitignored.

## Using Igneous in a Game

**Recommended:** git submodule + `cmake/Igneous.cmake` (three lines in your `CMakeLists.txt`):

```cmake
cmake_minimum_required(VERSION 3.30)
project(my_game LANGUAGES CXX)

include(external/igneous/cmake/Igneous.cmake)
igneous_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/igneous)
igneous_add_executable(my_game src/main.cpp)
```

Copy the starter layout from [`template/game/`](template/game/); `cmake/Igneous.cmake` also supports FetchContent, Steam, assets, and install options — see the comments at the top of each function in that file.

Minimal entry point:

```cpp
#include "igneous/engine/Engine.hpp"
#include "igneous/scenes/Scene.hpp"

class MainScene : public Engine::Scene {
public:
    void Update(double delta) override { /* ... */ }
    void Render() override { /* ... */ }
};

int main() {
    Engine::Engine engine;
    return engine.Run<MainScene>("Main");
}
```

## Project Layout

```
include/igneous/   Public headers
src/               Engine implementation
template/game/     Starter CMake project for new games
libs/              Third-party dependencies (git submodules)
cmake/             Igneous.cmake consumer API + helpers
```

## License

See individual dependency licenses under `libs/`. Engine license not specified in-repo — check with maintainers.
