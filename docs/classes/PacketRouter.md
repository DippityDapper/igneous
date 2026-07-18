# PacketRouter

**Header:** `include/igneous/networking/PacketRouter.hpp`  
**Namespace:** `Engine`  
**Tests:** `[networking][packetrouter]`, `[networking][serializer][header]`

## Overview

Dispatches incoming message payloads by [PacketType](PacketTypes.md). Handlers receive the peer ID and payload bytes **after** the wire header is stripped.

## API

| Method | Description |
|--------|-------------|
| `Connect(type, handler)` | Register handler; returns connection id |
| `Dispatch(type, peerId, data)` | Invoke handlers for a packet type |
| `DispatchMessage(peerId, data)` | Read header from payload and dispatch |
| `Disconnect(type, handlerId)` | Remove one handler |
| `Clear()` | Remove all handlers |

## Related

- [Serializer](Serializer.md#wire-format), [NetworkProtocol](../include/igneous/networking/NetworkProtocol.hpp)
