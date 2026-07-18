# Tech Debt

Known issues, shortcuts, and improvement areas in Igneous as of the current codebase. This is a living list — remove rows when resolved.

| Area | Description | Why it matters |
|------|-------------|----------------|
| **Examples are demos, not tests** | Sixteen standalone `example_*` targets under `examples/` demonstrate features but are not automated tests. | Regressions still require manual runs; no CI coverage of engine behavior. |
| **Steam stubs when disabled** | `SteamNetwork` and `SteamIdentity` compile to no-op stubs unless `IGNEOUS_STEAM_ENABLED` is set. Stub behavior is documented in [SteamNetwork.md](classes/SteamNetwork.md) and covered by `[networking][steam][stub]`. | Games still need `-DIGNEOUS_STEAM=ON` and the SDK for real Steam networking. |
| **Native-endian serialization** | `Serializer` / `Deserializer` write arithmetic types with native endianness — documented in [Serializer.md](classes/Serializer.md); no wire conversion layer yet. | Network messages are not portable across big-endian / little-endian peers without explicit conversion code. |
| **Limited test coverage** | `igneous_tests` covers core subsystems but not full SDL loops, ENet/Steam, or audio/render integration. | See [Tests](tests.md); examples still supplement manual integration checks. |
| **Large vendored submodules** | SDL3, ImGui, SQLiteCpp, ENet, miniupnp, etc. are full git submodules under `libs/`. | Long initial clone/build; submodule drift and security updates require manual tracking. |
| **Static manager globals** | `Input`, `Window`, `Renderer`, `ResourceManager`, `SceneManager`, `Time`, and `Camera::main` use static inline state. `ResetForTests()` / `RestoreBaseline()` helpers and a Catch2 listener isolate unit tests; full DI refactor is still out of scope. | Hard to run multiple engine instances in one process; see [Tests](tests.md#static-state-and-test-isolation). |

## Related Docs

- [Code Reference](code-reference.md) — intended conventions (debt items often violate or stretch these)
- [Architecture Index](architecture.md) — subsystem map
- [How to Build](how-to-build.md) — build and run workflow
