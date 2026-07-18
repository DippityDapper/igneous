# NetworkInterface

**Header:** `include/igneous/networking/NetworkInterface.hpp`  
**Namespace:** `Engine`

## Overview

Abstract networking transport. Messages arrive via `onMessageReceived` when `Poll()` is called.

## Virtual API

| Method | Description |
|--------|-------------|
| `SendToServer(data, flags)` | Client → server |
| `SendToClient(peerId, data, flags)` | Server → client |
| `Poll()` | Process queued messages |
| `Connected()` | Connection state |
| `Clean()` | Shutdown |

## Loopback

Same-process client/server pairs use [ILoopbackNetwork](ILoopbackNetwork.md) and [NetworkSessionFactory::LinkLoopback](NetworkSessionFactory.md). There is no `SetLoopbackPeer` on this interface.

## Implementations

- [LocalNetwork](LocalNetwork.md)
- [ENetNetwork](ENetNetwork.md)
- [SteamNetwork](SteamNetwork.md)
