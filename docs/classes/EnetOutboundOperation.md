# EnetOutboundOperation

**Header:** `include/igneous/networking/EnetOutboundOperation.hpp`  
**Namespace:** `Engine`

## Overview

Queued outbound work item for [ENetNetwork](ENetNetwork.md). The network thread drains these from an `Engine::ThreadSafeQueue` while the main thread enqueues sends and disconnects.

## Types

| Name | Description |
|------|-------------|
| `EnetOutboundKind` | `SendToServer`, `SendToClient`, `DisconnectPeer`, `GracefulClientDisconnect` |
| `EnetOutboundOperation` | `kind`, `peerId`, `data`, `transport` fields |
