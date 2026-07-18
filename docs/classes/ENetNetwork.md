# ENetNetwork

**Header:** `include/igneous/networking/ENetNetwork.hpp`  
**Implementation:** `src/networking/ENetNetwork.cpp`  
**Namespace:** `Engine`

UDP transport via ENet.

## Connect

| Method | Role |
|--------|------|
| `Connect(port, peerCount, localOnly, enableUpnp = false)` | Start server; UPnP mapping is opt-in via `enableUpnp` |
| `Connect(port, ip)` | Connect client |
| `Connect()` | Loopback client stub for listen-server |

## Features

- Background network thread with inbound `Engine::ThreadSafeQueue`
- Outbound queue (`EnetOutboundOperation`) — main thread enqueues sends
- `NetworkLoopbackLink` for listen-server local client
- `ClientConnectionWatchdog` / `ServerPeerActivityTracker`
- `NetworkPeerIds::FromEnetPeer` peer ID mapping
- Optional UPnP port mapping (background thread; pass `enableUpnp = true` on non-local servers)

## Send

Uses `TransportType::Reliable` / `TransportType::Unreliable` instead of raw ENet flags.

## See also

- [NetworkSessionFactory](NetworkSessionFactory.md)
- [LocalNetwork](LocalNetwork.md)
- [SteamNetwork](SteamNetwork.md)
