# How to Build

Complete instructions for building Igneous from the terminal or from VS Code / Cursor, and for running a game that uses the engine.

## What Gets Built

The root `CMakeLists.txt` produces:

| Target | Output |
|--------|--------|
| `igneous` | Static engine library (`build/libigneous.a`) |
| `example_*` | Sixteen feature demos (`build/examples/<name>/example_<name>`) — **ON by default** |
| `igneous_tests` | Unit tests (`build/tests/igneous_tests`) — **ON by default** |

Disable examples with `-DIGNEOUS_BUILD_EXAMPLES=OFF`. Disable tests with `-DIGNEOUS_BUILD_TESTS=OFF` if you only want the library.

See [Examples](examples/README.md) for demos and [Tests](tests.md) for the test index.

---

## 1. Clone and Submodules

Igneous vendors dependencies as git submodules under `libs/`.

```bash
git clone --recurse-submodules <repo-url> igneous
cd igneous
```

If you already cloned without submodules:

```bash
git submodule update --init --recursive
```

---

## 2. Prerequisites

### Linux (Arch / CachyOS / Debian / Fedora)

Install a C++20 toolchain and CMake 3.30+:

```bash
# Arch / CachyOS
sudo pacman -S base-devel cmake git

# Debian / Ubuntu
sudo apt install build-essential cmake git

# Fedora
sudo dnf install gcc-c++ cmake git
```

Optional but recommended:

```bash
# Arch
sudo pacman -S ninja

# Debian / Ubuntu
sudo apt install ninja-build
```

SDL3 and other deps are **built from source** inside `libs/` — you generally do not need system SDL dev packages.

### macOS

```bash
xcode-select --install
brew install cmake ninja
```

### Windows

- [Visual Studio 2022](https://visualstudio.microsoft.com/) with **Desktop development with C++**
- [CMake 3.30+](https://cmake.org/download/)
- Optional: [Ninja](https://github.com/ninja-build/ninja/releases) and add it to `PATH`
- Git for submodules

For MinGW builds, ensure `g++` and `cmake` are on `PATH`. `IgneousHelpers.cmake` copies MinGW runtime DLLs when targeting Windows.

---

## 3. Build from the Terminal

### Configure

From the repo root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

With Ninja (faster):

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

Release build:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

### Compile

```bash
cmake --build build
```

Parallel build:

```bash
cmake --build build -j$(nproc)    # Linux
cmake --build build -j %NUMBER_OF_PROCESSORS%   # Windows cmd
```

### Verify

```bash
# Linux / macOS
ls build/libigneous.a

# Windows (MSVC)
dir build\Debug\igneous.lib
```

### Optional: Enable Steamworks

1. Install the Steamworks SDK into `libs/steamworks/` (gitignored).
2. Re-configure:

```bash
cmake -B build -DIGNEOUS_STEAM=ON
cmake --build build
```

CMake fails fast with a clear error if SDK binaries are missing.

### Clean Rebuild

```bash
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

---

## 4. Build in VS Code / Cursor

### Extensions

Install these extensions in **Cursor** or **VS Code**:

1. **CMake Tools** (`ms-vscode.cmake-tools`) — configure, build, run target
2. **clangd** (`llvm-vs-code-extensions.vscode-clangd`) — IntelliSense (uses `compile_commands.json`)
3. **CodeLLDB** (`vadimcn.vscode-lldb`) — **required for F5 debugging in Cursor**

Optional in **VS Code only**: **C/C++** (`ms-vscode.cpptools`) — use the secondary launch config **Debug example_hello (VS Code C/C++)** if you prefer `cppdbg` over CodeLLDB. IntelliSense stays disabled in favor of clangd.

**Cursor note:** Cursor does not support the `cppdbg` debug type. Use the default **Debug example_hello** config (CodeLLDB / `lldb` type).

### Open the Project

Open the repo folder in Cursor (`cursor .` or **File → Open Folder**).

The repo ships **`CMakePresets.json`** for configure/build presets. IDE settings (`.vscode/`, `.cursor/`) are **not** checked in — create them locally if you want editor integration.

On first open:

1. Install the extensions listed above.
2. **CMake: Select a Kit** — pick your compiler (GCC, Clang, or MSVC).
3. **CMake: Select Configure Preset** → **`debug`** (from `CMakePresets.json`).
4. Pick any `example_*` target from the CMake status bar to run a demo.

For IntelliSense, enable **`CMAKE_EXPORT_COMPILE_COMMANDS`** (already set in presets) and point clangd at the repo root, or copy `build/compile_commands.json` to the root after configure.

### Configure

Presets are defined in `CMakePresets.json`:

| Preset | Build type |
|--------|------------|
| `debug` | Debug (default) |
| `release` | Release |

From the command palette: **CMake: Select Configure Preset** to switch.

Or from the terminal:

```bash
cmake --preset debug
cmake --build --preset debug
./build/examples/hello/example_hello
```

Other examples: `./build/examples/<folder>/example_<folder>` (e.g. `./build/examples/animation/example_animation`). Asset paths are resolved from the executable directory automatically.

### Build

- **CMake: Build** from the command palette, or
- Click **Build** in the CMake Tools status bar, or
- `F7` (default keybinding)

### Run and Debug

| Action | Cursor / VS Code |
|--------|------------------|
| **F5** (debug) | **Debug example_hello** — requires **CodeLLDB** extension |
| **Ctrl+F5** (run) | **CMake: Run Without Debugging** in the status bar, or task **Run example_hello** |
| **Build** | **F7** or **CMake: Build** |

Pick a different example from **CMake: Set Launch Target** (e.g. `example_animation`, `example_audio`).

After installing CodeLLDB, run **Developer: Reload Window**, then press **F5**.

If debugging still fails, pick **Debug example_hello** in the Run and Debug panel (not the VS Code C/C++ variant).

Runtime libraries (SDL, etc.) are copied next to the executable automatically via `igneous_copy_runtime_dependencies`.

### CLion

CLion does not use `.vscode/launch.json`. Open the repo as a **CMake project**:

1. **File → Open** → select the repo root (where `CMakeLists.txt` lives)
2. Wait for CMake profile indexing to finish
3. Select an **`example_*`** run configuration from the toolbar dropdown (e.g. `example_hello`)
4. Click **Run** or **Debug** (CLion uses GDB/LLDB built-in)

Ensure **CMake options** include `-DIGNEOUS_BUILD_EXAMPLES=ON` (default in presets; CLion may need the option set under **Settings → Build, Execution, Deployment → CMake** if targets are missing).

### Optional local VS Code / Cursor setup

Create `.vscode/settings.json` locally (gitignored) if you use **CMake Tools**:

```json
{
  "cmake.useCMakePresets": "always",
  "cmake.configurePreset": "debug",
  "cmake.buildPreset": "debug",
  "cmake.buildDirectory": "${workspaceFolder}/build",
  "cmake.launchTarget": "example_hello",
  "cmake.copyCompileCommands": "${workspaceFolder}/compile_commands.json",
  "C_Cpp.intelliSenseEngine": "disabled",
  "clangd.arguments": ["--compile-commands-dir=${workspaceFolder}"]
}
```

For **F5** debugging, add `.vscode/launch.json` with a **CodeLLDB** (`lldb`) configuration that launches `${command:cmake.launchTargetPath}` from `${command:cmake.launchTargetDirectory}`.

Re-run **CMake: Configure** after changing settings.

---

## 5. How to Run

Because Igneous is a library, **running** means building and launching a game executable that links it.

### Option A: Separate Game Repository

Use the [game template](../template/game/) or [Linking Igneous](linking.md). Minimal `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.30)
project(my_game LANGUAGES CXX)

include(external/igneous/cmake/Igneous.cmake)
igneous_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/igneous)
igneous_add_executable(my_game src/main.cpp)
```

Build and run:

```bash
git submodule add <igneous-repo-url> external/igneous
git submodule update --init --recursive
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/my_game
```

`igneous_add_executable` links the engine and copies SDL shared libraries next to your executable.

### Option B: In-Repo Examples

The repo ships sixteen standalone demos under `examples/`. Each builds as `example_<folder>` and copies its `assets/` folder next to the executable:

```bash
cmake -B build -DIGNEOUS_BUILD_EXAMPLES=ON
cmake --build build
./build/examples/hello/example_hello
./build/examples/animation/example_animation
```

See [Examples](examples/README.md) for the full list and controls.

### Run / Debug in VS Code / Cursor

1. Install **CodeLLDB** (`vadimcn.vscode-lldb`).
2. Select **Debug example_hello** (or another `example_*` config) in the Run panel.
3. Press **F5** to debug, or use **CMake: Run Without Debugging** to run without breakpoints.

If `IGNEOUS_BUILD_EXAMPLES=OFF`, only the library is built until you add your own executable.

### Steam Runtime Notes

When `IGNEOUS_STEAM=ON`:

- Place `steam_appid.txt` in your game's source directory; it is copied post-build automatically.
- Run the game binary from its output folder so `libsteam_api.so` / `steam_api64.dll` is found.

---

## 6. Tests

```bash
cmake --preset debug
cmake --build --preset debug --target igneous_tests
./build/tests/igneous_tests
```

Or run via CTest: `ctest --test-dir build --output-on-failure`.

See [Tests](tests.md) for the full index by engine system.

### CI

GitHub Actions (`.github/workflows/ci.yml`) builds and runs tests on every pull request and push to `master` / `main`. Locally, mirror the CI configure/build/test steps:

```bash
git submodule update --init --recursive
cmake --preset debug -DIGNEOUS_BUILD_TESTS=ON
cmake --build --preset debug --target igneous igneous_tests example_hello
ctest --test-dir build --output-on-failure
```

---

## 7. Troubleshooting

| Problem | Fix |
|---------|-----|
| Empty `libs/SDL3` | Run `git submodule update --init --recursive` |
| `CMake 3.30 required` | Upgrade CMake |
| Compiler not C++20 | Use GCC 11+, Clang 14+, or MSVC 2022+ |
| Game can't find `libSDL3.so` | Call `igneous_copy_runtime_dependencies`; run from build output dir |
| `IGNEOUS_STEAM` configure error | Install SDK to `libs/steamworks/` per SDK layout in root `CMakeLists.txt` |
| Slow first build | Normal — SDL and dependencies compile from source |
| `cppdbg is not supported` (Cursor) | Install **CodeLLDB**; use **Debug example_hello** launch config |
| Ninja not found | Install ninja or drop `-G Ninja` and use default Makefiles |

---

## 8. Install (Optional)

Install the engine library and CMake package to a prefix:

```bash
cmake --preset release
cmake --build --preset release
cmake --install build --prefix ~/.local/igneous
```

This installs headers, `libigneous.a`, `Igneous.cmake`, and `IgneousHelpers.cmake`.

Most games should use **`igneous_add_subdirectory`** (see [Linking Igneous](linking.md)) because it builds all vendored dependencies together. Install is mainly for packaging the engine library and headers.
