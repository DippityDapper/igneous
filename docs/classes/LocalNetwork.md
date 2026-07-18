# LocalNetwork

**Header:** `include/igneous/networking/LocalNetwork.hpp`  
**Implementation:** `src/networking/LocalNetwork.cpp`  
**Namespace:** `Engine`

## Overview

In-memory networking for single-process simulation. Pairs of `LocalNetwork` instances exchange messages through internal queues when `Poll()` runs.

## Constructor

`LocalNetwork(isServer)` — server or client role.

## Usage

Create two instances and link them with [NetworkSessionFactory::LinkLoopback](NetworkSessionFactory.md). No real sockets.

```cpp
Engine::LocalNetwork server;
Engine::LocalNetwork client;
Engine::NetworkSessionFactory::LinkLoopback(server, client);
```

## Related

- [NetworkLoopbackLink](NetworkLoopbackLink.md), [ILoopbackNetwork](ILoopbackNetwork.md)
