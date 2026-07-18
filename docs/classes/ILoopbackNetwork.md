# ILoopbackNetwork

**Header:** `include/igneous/networking/ILoopbackNetwork.hpp`  
**Namespace:** `Engine`

## Overview

Mixin interface for transports that support in-process loopback via [NetworkLoopbackLink](NetworkLoopbackLink.md). Implemented by `LocalNetwork`, `ENetNetwork`, and `SteamNetwork`.

## API

| Method | Description |
|--------|-------------|
| `GetLoopback()` | Mutable link used by [NetworkSessionFactory::LinkLoopback](NetworkSessionFactory.md) |
