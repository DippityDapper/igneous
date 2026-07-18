# LocalNetwork

**Header:** `include/igneous/networking/LocalNetwork.hpp`  
**Implementation:** `src/networking/LocalNetwork.cpp`  
**Namespace:** `Engine`

## Overview

In-memory networking for single-process simulation. Pairs of `LocalNetwork` instances exchange messages through internal queues when `Poll()` runs.

## Constructor

`LocalNetwork(isServer)` — server or client role.

## Usage

Create two instances, call `SetLoopbackPeer` on each to point at the other. No real sockets.
