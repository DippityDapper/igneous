# SteamNetwork

**Header:** `include/igneous/networking/SteamNetwork.hpp`  
**Implementation:** `src/networking/SteamNetwork.cpp`  
**Namespace:** `Engine`

## Overview

Steam Networking Sockets transport. Uses Steam relay instead of direct UDP/ENet.

Requires `IGNEOUS_STEAM=ON` at configure time, which defines `IGNEOUS_STEAM_ENABLED` and links the Steamworks SDK.

## Stub behavior (Steam disabled)

When `IGNEOUS_STEAM_ENABLED` is **not** defined, the class compiles to no-op stubs:

- `Connect()` / `Connect(hostSteamId)` log **once** to SDL_Log and return without connecting.
- `SendToServer`, `SendToClient`, and `Poll` are no-ops.
- `Connected()` always returns `false`.
- `Clean()` is safe to call.

This lets game code link against `SteamNetwork` in development builds without the Steamworks SDK. See [How to Build](../how-to-build.md) for enabling Steam.

## Connect (Steam enabled)

| Signature | Role |
|-----------|------|
| `Connect()` | Listen server (P2P) |
| `Connect(hostSteamId)` | Client connect; `hostSteamId == 0` selects loopback stub |
| `()` | Used with `NetworkSessionFactory::CreateSteamClientServer()` loopback pairing |

## Threading

Dedicated network thread; messages delivered via `Engine::ThreadSafeQueue` in `Poll()`.

## Tests

| Tag | When |
|-----|------|
| `[networking][steam][stub]` | Default CI (Steam off) — stub `Connect()` / `Connected()` |
