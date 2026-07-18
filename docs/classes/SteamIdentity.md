# SteamIdentity

**Header:** `include/igneous/networking/SteamIdentity.hpp`  
**Implementation:** `src/networking/SteamIdentity.cpp`  
**Namespace:** `Engine`

## Overview

Steamworks-backed identity using SteamID and persona name. Server validates clients via Steam auth session tickets.

Requires `IGNEOUS_STEAM_ENABLED` at compile time. Without Steam, methods are stubs.

## Key Behavior

- `GenerateLocalId()` / `GenerateLocalUsername()` — cache from Steam API
- `GetAuthToken(remoteId)` — session ticket bound to server SteamID
- `ValidateToken()` — async via `ValidateAuthTicketResponse_t` → `OnAuthResult`
- `OnDisconnect()` — end auth session for client
