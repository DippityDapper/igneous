# Igneous Game Template

Minimal CMake project that links Igneous as a submodule.

## Setup

From a new or existing game repository:

```bash
# 1. Copy this template (or use it as a starting layout)
cp -r path/to/igneous/template/game/* .

# 2. Add Igneous as a submodule
git submodule add <igneous-repo-url> external/igneous
git submodule update --init --recursive

# 3. Build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/my_game
```

## Layout

```
my_game/
  CMakeLists.txt
  src/main.cpp
  external/igneous/    # git submodule → Igneous engine
  assets/              # optional — enable ASSETS in CMakeLists.txt
```

## CMake API

This template uses `cmake/Igneous.cmake` from the engine:

| Function | Purpose |
|----------|---------|
| `igneous_add_subdirectory(dir)` | Add engine; disables examples/tests by default |
| `igneous_add_executable(target sources...)` | Create game exe, link engine, copy SDL DLLs |
| `igneous_link_target(target)` | Link an existing target to Igneous |

See `../cmake/Igneous.cmake` for FetchContent, Steam, and install options (documented in comments above each function).

## Steam (optional)

```cmake
igneous_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/igneous STEAM ON)
```

Place the Steamworks SDK under `external/igneous/libs/steamworks/` before configuring.
