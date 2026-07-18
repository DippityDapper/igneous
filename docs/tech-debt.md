# Tech Debt

Known issues, shortcuts, and improvement areas in Igneous as of the current codebase. This is a living list — remove rows when resolved.

| Area | Description | Why it matters |
|------|-------------|----------------|
| **Examples are demos, not tests** | Sixteen standalone `example_*` targets under `examples/` demonstrate features but are not automated tests. | Regressions still require manual runs; no CI coverage of engine behavior. |
| **Incomplete header docs** | `ResourceManager` and `AudioStream` use empty Doxygen blocks (`/** */`) while other types are fully documented. | API discovery relies on reading implementation; docs in `docs/classes/` fill the gap manually but headers are inconsistent. |
| **Hardcoded window defaults** | `Engine::InitSDL()` creates a 640×360 window titled `"client"` with no public API to change it at init. | Every game gets the same window unless Window is reconfigured elsewhere; not suitable as a library default long-term. |
| **`CFGParser::GetUInt16` return type** | Method is named and documented as `uint16_t` but declared and implemented as `uint32_t`. | Callers may assume 16-bit range; silent widening hides the API mistake. |
| **`Vec2` / `Vec3` math precision** | `Dot()` and `Magnitude()` accumulate using `int` intermediates (`x * rhs.x`, `std::pow(x, 2)`) even for floating-point instantiations. | Large or fractional values lose precision or overflow; float vectors do not get correct math. |
| **`ThreadSafeQueue` namespace** | Template lives in the global namespace while the rest of the engine uses `Engine::`. | Inconsistent include/usage pattern; pollutes global scope. |
| **Missing include in `IdentityProvider.hpp`** | Uses `std::vector<uint8_t>` without `#include <vector>`. | Compiles only when another header pulls in `<vector>`; fragile and non-self-contained. |
| **Steam stubs when disabled** | `SteamNetwork` and `SteamIdentity` compile to no-op stubs unless `IGNEOUS_STEAM_ENABLED` is set. | Easy to link against Steam types in development and only discover missing behavior at runtime or integration time. |
| **Native-endian serialization** | `Serializer` / `Deserializer` write arithmetic types with `reinterpret_cast` — platform byte order, no version header. | Network messages are not portable across big-endian / little-endian peers without an explicit wire format. |
| **`SceneRoot::AddScene` cast** | Returns `reinterpret_cast<T*>` after `std::make_unique<T>()` instead of `static_cast<T*>`. | Works in practice but bypasses type safety; unnecessary undefined-behavior risk if refactored incorrectly. |
| **Private virtual scene hooks** | `Scene` declares `OnCreated`, `Update`, `Render`, etc. as **private** virtual methods. | Valid C++ (derived classes can override with public methods) but unconventional; confuses readers and tooling expecting protected hooks. |
| **Limited test coverage** | `igneous_tests` covers core subsystems but not full SDL loops, ENet/Steam, or audio/render integration. | See [Tests](tests.md); examples still supplement manual integration checks. |
| **`Perlin` header-only implementation** | Full noise generation, PNG I/O, and surface building live in `PerlinNoise.hpp`. | Long compile times for any TU that includes it; mixes algorithm, rendering, and file I/O in one header. |
| **Large vendored submodules** | SDL3, ImGui, SQLiteCpp, ENet, miniupnp, etc. are full git submodules under `libs/`. | Long initial clone/build; submodule drift and security updates require manual tracking. |
| **`Event` ignores return values** | Callback return types are accepted but always discarded on `Emit()`. | Misleading template parameter; async or cancellable event patterns need a different abstraction. |
| **Sprite ID random assignment** | `ResourceManager::RegisterSprite` loops on random `int` IDs until unique. | Theoretically unbounded loop under collision; no deterministic ID strategy for debugging or saves. |
| **UPnP in `RemoteNetwork`** | Server path integrates miniupnpc for port mapping. | Adds failure modes and platform/router variance; not all deployments want automatic UPnP. |
| **Static manager globals** | `Input`, `Window`, `Renderer`, `ResourceManager`, `SceneManager`, `Time`, and `Camera::main` use static inline state. `ResetForTests()` helpers and a Catch2 listener isolate unit tests; full DI refactor is still out of scope. | Hard to run multiple engine instances in one process; see [Tests](tests.md#static-state-and-test-isolation). |
| **Window resize handling split** | Engine handles `SDL_EVENT_WINDOW_RESIZED` directly; `Input::wasWindowResized` is a separate path. | Two mechanisms for the same concern; easy to miss one when adding features. |
| **Deserializer default offset** | `Deserializer` skips the first 2 bytes by default (`startOffset = 2`). | Implicit protocol assumption; undocumented in the wire format and easy to get wrong when adding message types. |

## Related Docs

- [Code Reference](code-reference.md) — intended conventions (debt items often violate or stretch these)
- [Architecture Index](architecture.md) — subsystem map
- [How to Build](how-to-build.md) — build and run workflow
