# ClientConnectionWatchdog

**Header:** `include/igneous/networking/ClientConnectionWatchdog.hpp`  
**Namespace:** `Engine`  
**Tests:** `[networking][clientconnectionwatchdog]`

## Overview

Client-side keepalive helper for [ENetNetwork](ENetNetwork.md). Tracks last activity, emits periodic ping packets, and detects connection timeout.

## API

| Member / method | Description |
|-----------------|-------------|
| `PingPacket` | Static ping payload bytes |
| `timeoutSeconds` | Disconnect after idle (default 30) |
| `pingIntervalSeconds` | Ping cadence (default 1) |
| `MarkActivity(now)` | Reset idle timer |
| `HasTimedOut(now)` | True when enabled and idle too long |
| `TryConsumePingSend(now)` | True when a ping should be sent |
