# ServerPeerActivityTracker

**Header:** `include/igneous/networking/ServerPeerActivityTracker.hpp`  
**Namespace:** `Engine`  
**Tests:** `[networking][serverpeeractivitytracker]`

## Overview

Server-side peer activity tracking for [ENetNetwork](ENetNetwork.md). Records last-seen times per peer and collects stale peers for timeout handling.

## API

| Method | Description |
|--------|-------------|
| `TrackPeer(peerId, now)` | Register new peer activity |
| `MarkActivity(peerId, now)` | Refresh peer timestamp |
| `RemovePeer(peerId)` | Drop tracking entry |
| `CollectStalePeerIds(now)` | Peers idle longer than `timeoutSeconds` |
| `TryConsumePingSend(now)` | Rate-limit server ping broadcasts |
