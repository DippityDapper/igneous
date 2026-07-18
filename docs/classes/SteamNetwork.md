# SteamNetwork

**Header:** `include/igneous/networking/SteamNetwork.hpp`  
**Implementation:** `src/networking/SteamNetwork.cpp`  
**Namespace:** `Engine`

## Overview

Steam Networking Sockets transport. Mirrors `RemoteNetwork` API but uses Steam relay instead of direct ENet.

Requires `IGNEOUS_STEAM_ENABLED`. Without Steam, constructors and methods are no-op stubs.

## Constructors

| Signature | Role |
|-----------|------|
| `(port, localOnly)` | Listen server |
| `(port, ip)` | Client connect |
| `()` | Loopback stub |

## Threading

Dedicated network thread; messages delivered via `ThreadSafeQueue` in `Poll()`.
