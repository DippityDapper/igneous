# Linking Igneous in a Game

Igneous is designed to be consumed as a **CMake subproject**. The recommended path is a git submodule plus `cmake/Igneous.cmake` — one include, one link call.

## Quick start (submodule)

```bash
mkdir my_game && cd my_game
git init
cp -r /path/to/igneous/template/game/* .
git submodule add <igneous-repo-url> external/igneous
git submodule update --init --recursive
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/my_game
```

Or copy the layout from [`template/game/`](../template/game/).

## CMake API

Include the consumer module **before** adding the engine:

```cmake
include(external/igneous/cmake/Igneous.cmake)
igneous_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/igneous)
igneous_add_executable(my_game src/main.cpp)
```

### `igneous_add_subdirectory(source-dir)`

Adds Igneous via `add_subdirectory`. When embedded in a game:

- **Examples OFF** by default (`IGNEOUS_BUILD_EXAMPLES`)
- **Tests OFF** by default (`IGNEOUS_BUILD_TESTS`)

Optional flags:

```cmake
igneous_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/igneous
    BUILD_EXAMPLES    # build engine demos too
    BUILD_TESTS       # build igneous_tests
    STEAM ON          # enable Steamworks (SDK required)
)
```

### `igneous_add_executable(target [sources...] [ASSETS dir])`

Creates an executable, links `igneous::igneous`, copies SDL/shared libs next to the binary, and optionally copies an assets folder:

```cmake
igneous_add_executable(my_game src/main.cpp ASSETS assets)
```

### `igneous_link_target(target [ASSETS dir])`

Link an existing target (if you prefer your own `add_executable`):

```cmake
add_executable(my_game src/main.cpp)
igneous_link_target(my_game ASSETS assets)
```

### `igneous_fetch_content(GIT_REPOSITORY … [GIT_TAG …])`

Download Igneous with submodules via FetchContent. **Include `Igneous.cmake` first** from a submodule checkout or copy of the engine repo:

```cmake
include(external/igneous/cmake/Igneous.cmake)

igneous_fetch_content(
    GIT_REPOSITORY https://github.com/your-org/igneous.git
    GIT_TAG v0.1.0
)

igneous_add_executable(my_game src/main.cpp)
```

`GIT_TAG` defaults to `main`. Examples and tests are forced OFF during fetch.

## Target alias

The engine exports **`igneous::igneous`** (alias of `igneous`). Manual linking:

```cmake
target_link_libraries(my_game PRIVATE igneous::igneous)
include(external/igneous/cmake/IgneousHelpers.cmake)
igneous_copy_runtime_dependencies(my_game ${CMAKE_CURRENT_SOURCE_DIR})
```

## Recommended repository layout

```
my_game/
  CMakeLists.txt
  src/
  assets/              # optional
  external/
    igneous/           # git submodule
```

Avoid sibling paths like `../igneous` — they break when clones move. Submodules or FetchContent pin a reproducible path.

## Top-level vs embedded defaults

| Context | Examples | Tests |
|---------|----------|-------|
| Building Igneous repo directly | ON | ON |
| `igneous_add_subdirectory` in a game | OFF | OFF |

Override with `BUILD_EXAMPLES` / `BUILD_TESTS` or CMake cache flags.

## Install (optional)

Install copies the static library, headers, and CMake helper modules to a prefix:

```bash
cmake --preset release
cmake --build --preset release
cmake --install build --prefix ~/.local/igneous
```

This does **not** replace `add_subdirectory` for most games — vendored dependencies (SDL3, ImGui, etc.) are built as part of the Igneous tree. Use install for packaging or CI artifacts; use **`igneous_add_subdirectory`** for day-to-day game development.

## Steamworks

```cmake
igneous_add_subdirectory(... STEAM ON)
```

Install SDK under `external/igneous/libs/steamworks/`. Add `steam_appid.txt` to your game source dir; `igneous_copy_runtime_dependencies` copies it when Steam is enabled.

## Bootstrap script (engine repo)

From the Igneous repository root:

```bash
./scripts/setup.sh
```

Initializes submodules, configures with the `debug` preset, and builds.

## Requirements

Your game project needs the same toolchain as Igneous:

- CMake **3.30+**
- C++ **20** compiler
- Git (for submodules)

See [How to Build](how-to-build.md) for platform packages and IDE notes.
