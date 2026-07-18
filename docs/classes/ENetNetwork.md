# ENetNetwork

**Header:** `include/igneous/networking/ENetNetwork.hpp`  
**Implementation:** `src/networking/ENetNetwork.cpp`  
**Namespace:** `Engine`

UDP transport via ENet. Matches Tower of Riz `ENetNetwork` (formerly Igneous `RemoteNetwork`).

## Connect

| Method | Role |
|--------|------|
| `Connect(port, peerCount, localOnly)` | Start server |
| `Connect(port, ip)` | Connect client |
| `Connect()` | Loopback client stub for listen-server |

## Features

- Background network thread with inbound `ThreadSafeQueue`
- Outbound queue (`EnetOutboundOperation`) — main thread enqueues sends
- `NetworkLoopbackLink` for listen-server local client
- `ClientConnectionWatchdog` / `ServerPeerActivityTracker`
- `NetworkPeerIds::FromEnetPeer` peer ID mapping
- Optional UPnP port mapping (background thread)

## Send

Uses `TransportType::Reliable` / `TransportType::Unreliable` instead of raw ENet flags.

## See also

- [NetworkSessionFactory](NetworkSessionFactory.md)
- [LocalNetwork](LocalNetwork.md)
- [SteamNetwork](SteamNetwork.md)
