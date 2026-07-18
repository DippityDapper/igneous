# Examples

Standalone programs under `examples/` demonstrate one engine feature each. Every example has its own `main.cpp` and `assets/` folder (copied next to the executable at build time).

## Build and run

```bash
cmake --preset debug
cmake --build --preset debug
./build/examples/hello/example_hello
```

Replace `hello` with any example folder name. The engine sets the working directory to the executable folder at startup, so `assets/` paths work whether you launch from the repo root, the build tree, or the IDE.

Use **CMake: Run** in Cursor with the desired launch target (e.g. `example_animation`).

Set `-DIGNEOUS_BUILD_EXAMPLES=OFF` to build only the library.

## Spritesheet layout

Several examples use `colonist_*_spritesheet.png` (64×128 px):

| Grid | Meaning |
|------|---------|
| **8 rows** | Animations (top → bottom) |
| **4 columns** | Frames within an animation (left → right) |

Row order: idle down, idle right, idle left, idle up, walk down, walk right, walk left, walk up. Idle rows use 2 frames; walk rows use 4.

Use `Animation::AddFrame(path, 8, 4, animRow, frameColumn)`.

## Index

| Example | Feature | Doc |
|---------|---------|-----|
| [hello](hello.md) | Engine loop, Scene, ImGui, Time | Minimal entry point |
| [static_sprite](static_sprite.md) | Sprite | Single `crew_1.png` image |
| [animation](animation.md) | Animation, spritesheet | WASD + Shift walk cycles |
| [audio](audio.md) | AudioStream, ResourceManager | Button clicks and footsteps |
| [camera](camera.md) | Camera | Pan, zoom, bounds |
| [input_keyboard](input_keyboard.md) | Input | Keyboard state queries |
| [input_mouse](input_mouse.md) | Input | Mouse position, wheel, buttons |
| [input_gamepad](input_gamepad.md) | Input | Gamepad axes, buttons, rumble |
| [input_layers](input_layers.md) | InputLayer | Layer priority and consumption |
| [input_actions](input_actions.md) | InputMap | Actions with mixed device bindings |
| [scenes](scenes.md) | Scene, SceneManager, SceneRoot | Menu ↔ game scene swap |
| [z_index](z_index.md) | ResourceManager | Draw order / z-index |
| [renderer](renderer.md) | Renderer | World vs screen-space draws |
| [cfg](cfg.md) | CFGParser | Load/save key-value settings |
| [database](database.md) | Database | SQLite persistence |
| [events](events.md) | Event | Typed publish–subscribe |
| [perlin](perlin.md) | Perlin | Noise generation and preview |
| [networking_local](networking_local.md) | LocalNetwork, PacketRouter | In-process loopback |
| [networking_enet](networking_enet.md) | ENetNetwork | Localhost listen-server |
| [networking_steam](networking_steam.md) | SteamNetwork | P2P loopback (Steam optional) |
| [identity](identity.md) | LocalIdentity | Local user id and auth token |
| [thread_pool](thread_pool.md) | ThreadPool | Background task queue |

## Not covered as standalone examples

These types are internal or need external setup:

- **Window** — initialized by Engine in every example
- **AnimationFrame** — used inside [animation](animation.md)
- **InputEvent** — shown via [input_layers](input_layers.md) (handled flag / consumption)
- **InputLayer** — [input_layers](input_layers.md)
- **InputMap / InputAction** — [input_actions](input_actions.md)
- **SteamNetwork** — requires Steamworks SDK (`-DIGNEOUS_STEAM=ON`)
- **EnetLanDiscovery / entity sync packets** — game-layer features in `tower-of-riz/`, not in engine examples
- **ThreadSafeQueue** — used internally by networking backends
- **Vec2 / Vec3** — used throughout rendering and movement examples

## CMake target names

Each example builds as `example_<folder>` (e.g. `example_hello`, `example_audio`).
