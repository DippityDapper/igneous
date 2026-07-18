# NetworkEvents

**Header:** `include/igneous/networking/NetworkEvents.hpp`  
**Namespace:** `Engine`

## NetworkEventType

Enum describing queued network events:

| Value | Description |
|-------|-------------|
| `Message` | Payload data |
| `Ping` | Keepalive |
| `ClientConnected` | Client joined (server) |
| `ClientDisconnected` | Client left |
| `ServerDisconnected` | Lost server connection |
| `ConnectionSuccess` | Client connect succeeded |
| `ConnectionFailure` | Client connect failed |

## NetworkMessage

Struct carrying event data through `NetworkInterface::Poll()`:

| Member | Description |
|--------|-------------|
| `type` | `NetworkEventType` |
| `peerId` | Logical peer identifier |
| `data` | Payload bytes (for `Message`) |
| `flags` | ENet packet flags |
