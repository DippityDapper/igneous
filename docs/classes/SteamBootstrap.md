# SteamBootstrap

**Header:** `include/igneous/networking/SteamBootstrap.hpp`  
**Implementation:** `src/networking/SteamBootstrap.cpp` (when Steam enabled)  
**Namespace:** `Engine`

## Overview

Steam API lifecycle wrapper. [Engine](Engine.md) calls `Init`, `RunCallbacks`, and `Shutdown` when `IGNEOUS_STEAM_ENABLED` is set.

## API

| Method | Description |
|--------|-------------|
| `Init()` | Initialize Steam API |
| `RunCallbacks()` | Pump Steam callbacks each frame |
| `Shutdown()` | Release Steam API |
| `IsInitialized()` | Whether Steam is active |

## Related

- [SteamNetwork](SteamNetwork.md), [SteamIdentity](SteamIdentity.md)
