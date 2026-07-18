# IdentityProvider

**Header:** `include/igneous/networking/IdentityProvider.hpp`  
**Namespace:** `Engine`

## Overview

Abstract player identity and authentication interface. Implementations provide local IDs, usernames, auth tokens, and token validation.

## Callback

| Member | Description |
|--------|-------------|
| `OnAuthResult(userId, success)` | Async auth result (Steam) |

## Virtual API

| Method | Description |
|--------|-------------|
| `GetLocalId()` | Local player ID |
| `GetLocalUsername()` | Display name |
| `GenerateLocalId()` | Initialize/cache local ID |
| `GenerateLocalUsername()` | Initialize/cache username |
| `GetAuthToken(remoteId)` | Token bytes for connecting to remote |
| `ValidateToken(claimedId, token)` | Server-side validation |
| `OnDisconnect(id)` | Clean up session for disconnected peer |
| `Clean()` | Release resources |

## Implementations

- [LocalIdentity](LocalIdentity.md)
- [SteamIdentity](SteamIdentity.md)
