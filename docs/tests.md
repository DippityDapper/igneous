# Tests

Unit tests for Igneous engine systems and subsystems. Tests live under `tests/` at the project root and are **not** listed in `docs/classes/`.

## CI

Pull requests and pushes to `master` / `main` run [`.github/workflows/ci.yml`](../.github/workflows/ci.yml):

1. Initialize submodules
2. Configure with the `debug` preset, `-DIGNEOUS_BUILD_TESTS=ON`, and `-DSDL_UNIX_CONSOLE_BUILD=ON` (headless SDL on Linux CI runners)
3. Build `igneous`, `igneous_tests`, and `example_hello`
4. Run `ctest --test-dir build --output-on-failure`

## Build and run

```bash
cmake --preset debug -DIGNEOUS_BUILD_TESTS=ON
cmake --build --preset debug --target igneous_tests
./build/tests/igneous_tests

# or via CTest
ctest --test-dir build --output-on-failure
```

Disable tests with `-DIGNEOUS_BUILD_TESTS=OFF`.

Tests use [Catch2](https://github.com/catchorg/Catch2) (fetched automatically by CMake).

Filter by tag:

```bash
./build/tests/igneous_tests "[input]"
./build/tests/igneous_tests "[networking][serializer]"
```

## Static state and test isolation

Static singletons (`Input`, `Window`, `Renderer`, `CFGParser`, `SceneManager`, `ResourceManager`, `Time`, `Camera::main`) can leak state between Catch2 test cases. Use the shared reset helpers when a test mutates engine globals:

| Helper | Location | When to use |
|--------|----------|-------------|
| `Engine::TestReset::All()` | `tests/TestReset.hpp` | Resets every subsystem below; called automatically before and after each test case via a Catch2 listener |
| `Engine::Input::ResetForTests()` | `Input.hpp` | Input layers, devices, events, or action map |
| `Engine::Window::ResetForTests()` | `Window.hpp` | Viewport or SDL window |
| `Engine::Renderer::ResetForTests()` | `Renderer.hpp` | SDL renderer / ImGui context |
| `Engine::CFGParser::ResetForTests()` | `CFGParser.hpp` | Loaded config tables |
| `Engine::SceneManager::ResetForTests()` | `SceneManager.hpp` | Scene root singleton |
| `Engine::ResourceManager::ResetForTests()` | `ResourceManager.hpp` | Sprites, textures, sounds, mixer |
| `Engine::Time::ResetForTests()` | `Time.hpp` | Tick / delta fields |
| `Engine::Camera::ResetForTests()` | `Camera.hpp` | `Camera::main` pointer |

**Convention:** tests that need SDL video/audio/gamepad should use `SdlFixture` (`tests/SdlFixture.hpp`), which calls `TestReset::All()` and `Input::Init()` in setup. The test runner sets `SDL_VIDEODRIVER=dummy` when unset so CI and headless shells can create windows without a display server.

Add new tests under `tests/` and register the file in `tests/CMakeLists.txt`. If a test touches static managers directly, rely on the global listener or call `Engine::TestReset::All()` explicitly in fixture setup/teardown.

## Test index

### Engine (`tests/engine_test.cpp`)

| Tag | Subsystem | What is verified |
|-----|-----------|------------------|
| `[engine][vec2]` | Vec2 | Arithmetic, magnitude, distance, normalize |
| `[engine][vec3]` | Vec3 | Arithmetic |
| `[engine][event]` | Event | Connect, emit, disconnect |
| `[engine][cfgparser]` | CFGParser | Load, read, write, save |
| `[engine][database]` | Database | Open, execute, query |
| `[engine][perlin]` | Perlin | Seeded noise determinism, FBM range |
| `[engine][threadpool]` | ThreadPool | Enqueued work completes |
| `[engine][threadsafqueue]` | ThreadSafeQueue | Push / pop |
| `[engine][time]` | Time | Static tick fields |
| `[engine][camera]` | Camera | Bounds clamping |

### Input (`tests/input_test.cpp`)

| Tag | Subsystem | What is verified |
|-----|-----------|------------------|
| `[input][gamepadaxis]` | GamepadAxis | String conversion |
| `[input][inputlayer]` | InputLayer | Name, priority, `Is` |
| `[input][inputevent]` | InputEvent | Default state |
| `[input][inputmap]` | InputMap | Actions, bindings, remove |
| `[input][inputaction]` | InputAction | Empty action value |
| `[input][inputmapquery]` | InputMapQuery | Axis composition |
| `[input][inputmaploader]` | InputMapLoader | JSON load / save round trip |
| `[input][input]` | Input | Layer priority sort |

### Rendering (`tests/rendering_test.cpp`)

| Tag | Subsystem | What is verified |
|-----|-----------|------------------|
| `[rendering][window]` | Window | Viewport after init |
| `[rendering][renderer]` | Renderer | Init / clean with window |

### Scenes (`tests/scenes_test.cpp`)

| Tag | Subsystem | What is verified |
|-----|-----------|------------------|
| `[scenes][sceneroot]` | SceneRoot | Add, load, unload, deferred removal |
| `[scenes][scenemanager]` | SceneManager | Init / remove root |
| `[scenes][scene]` | Scene | Input dispatch by layer |

### Resources (`tests/resources_test.cpp`)

| Tag | Subsystem | What is verified |
|-----|-----------|------------------|
| `[resources][animationframe]` | AnimationFrame | Atlas fields |
| `[resources][animation]` | Animation | FPS frame advance |
| `[resources][sprite]` | Sprite | Named animations |
| `[resources][resourcemanager]` | ResourceManager | Scale mode API |

### Networking (`tests/networking_test.cpp`)

| Tag | Subsystem | What is verified |
|-----|-----------|------------------|
| `[networking][serializer]` | Serializer / Deserializer | Primitive round trip |
| `[networking][localidentity]` | LocalIdentity | User file parsing |
| `[networking][localnetwork]` | LocalNetwork | Loopback peer link |
| `[networking][networkloopbacklink]` | NetworkLoopbackLink | Forward to callback |
| `[networking][packetrouter]` | PacketRouter | Dispatch, disconnect |
| `[networking][clientconnectionwatchdog]` | ClientConnectionWatchdog | Timeout, ping interval |
| `[networking][serverpeeractivitytracker]` | ServerPeerActivityTracker | Stale peer collection |
| `[networking][networkpeerids]` | NetworkPeerIds | ID constants / mapping |

## Not covered (integration / optional)

These need full SDL loops, external services, or optional SDKs and are exercised via [examples](examples/README.md) instead:

- **Engine** — `Engine::Run` main loop
- **Input** — live device polling with ImGui capture (`InputBinding` integration)
- **Resources** — `AudioStream`, texture loading, sprite rendering
- **Networking** — `ENetNetwork`, `SteamNetwork`, `NetworkSessionFactory`, LAN discovery
- **Identity** — `SteamIdentity`

Add new tests under `tests/` and register the file in `tests/CMakeLists.txt`. Update this document when adding coverage.
