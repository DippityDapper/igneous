# networking_enet

**Target:** `example_networking_enet`  
**Feature:** ENetNetwork listen-server on localhost

Creates an ENet server on port `33333` with a loopback client in the same process (Tower of Riz listen-server pattern).

## Run

```bash
./build/examples/networking_enet/example_networking_enet
```

## Controls

- **Client -> Server** — send packet over loopback through ENet server
- **Escape** — quit

## Notes

Uses `NetworkSessionFactory::CreateEnetClientServer(true)` — server binds `127.0.0.1`.
