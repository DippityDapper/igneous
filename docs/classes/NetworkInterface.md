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

`SetLoopbackPeer(peer)` pairs two interfaces for same-process testing.

## Implementations

- [LocalNetwork](LocalNetwork.md)
- [RemoteNetwork](RemoteNetwork.md)
- [SteamNetwork](SteamNetwork.md)
