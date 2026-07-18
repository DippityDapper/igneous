# NetworkPeerIds

**Header:** `include/igneous/networking/NetworkPeerIds.hpp`  
**Namespace:** `Engine`  
**Tests:** `[networking][networkpeerids]`

## Overview

Logical peer ID constants and ENet-to-engine ID mapping used by [ENetNetwork](ENetNetwork.md).

## API

| Name | Value / behavior |
|------|------------------|
| `Invalid` | `0` |
| `Local` | `1` (loopback client) |
| `FromEnetPeer(enetPeerId)` | Maps ENet peer index to engine peer id |
