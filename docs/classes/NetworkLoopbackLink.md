# NetworkLoopbackLink

**Header:** `include/igneous/networking/NetworkLoopbackLink.hpp`  
**Namespace:** `Engine`  
**Tests:** `[networking][networkloopbacklink]`

## Overview

Forwards `NetworkMessage` payloads to a paired `NetworkInterface` for same-process listen-server or test setups. Used by `ILoopbackNetwork` implementations (`LocalNetwork`, `ENetNetwork`, `SteamNetwork`).

Pair peers with [NetworkSessionFactory::LinkLoopback](NetworkSessionFactory.md), not a removed `SetLoopbackPeer` on [NetworkInterface](NetworkInterface.md).

## API

| Method | Description |
|--------|-------------|
| `IsLinked()` | Whether a peer is set |
| `SetPeer(interface*)` | Point at the paired transport |
| `Clear()` | Remove peer link |
| `Forward(message)` | Deliver message to peer's receive path |
