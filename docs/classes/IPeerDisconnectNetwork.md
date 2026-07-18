# IPeerDisconnectNetwork

**Header:** `include/igneous/networking/IPeerDisconnectNetwork.hpp`  
**Namespace:** `Engine`

## Overview

Optional interface for server transports that can force-disconnect a client by peer ID. Implemented by `ENetNetwork`.

## API

| Method | Description |
|--------|-------------|
| `DisconnectPeer(peerId)` | Drop the given logical peer |
