# NetworkSessionFactory

**Header:** `include/igneous/networking/NetworkSessionFactory.hpp`  
**Namespace:** `Engine`

## Overview

Factory helpers for paired client/server sessions and loopback wiring. Use `LinkLoopback` to connect two `ILoopbackNetwork` transports in the same process.

## Loopback linking

```cpp
Engine::LocalNetwork server;
Engine::LocalNetwork client;
Engine::NetworkSessionFactory::LinkLoopback(server, client);
```

Both types must implement [ILoopbackNetwork](ILoopbackNetwork.md) and [NetworkInterface](NetworkInterface.md).

## Session helpers

| Method | Returns |
|--------|---------|
| `CreateLocalClientServer()` | Two linked `LocalNetwork` instances (server + client) |
| `CreateEnetClientServer(localOnly)` | Linked `ENetNetwork` server and client |
| `CreateSteamClientServer()` | Linked `SteamNetwork` server and client (requires Steam) |

Constants: `EnetPort` (33333), `EnetPeerCount` (20).

## Related

- [NetworkLoopbackLink](NetworkLoopbackLink.md), [LocalNetwork](LocalNetwork.md), [ENetNetwork](ENetNetwork.md)
