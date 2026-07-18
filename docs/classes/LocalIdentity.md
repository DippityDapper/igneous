# LocalIdentity

**Header:** `include/igneous/networking/LocalIdentity.hpp`  
**Implementation:** `src/networking/LocalIdentity.cpp`  
**Namespace:** `Engine`

## Overview

File-backed local identity for offline or LAN play. Reads/writes user ID and username from a config file path.

## Constructor

`LocalIdentity(userFilePath)` — path to user data file.

## Behavior

- Generates and persists local ID and username when missing
- `GetAuthToken` / `ValidateToken` provide simple local auth (no Steam)
