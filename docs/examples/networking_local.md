# networking_local

**Target:** `example_networking_local`  
**Feature:** LocalNetwork, NetworkSessionFactory, PacketRouter, Serializer

In-process client/server using loopback links. Demonstrates typed packets with a `PacketType` header.

## Run

```bash
./build/examples/networking_local/example_networking_local
```

## Controls

- **Client -> Server** — send a demo chat packet; server replies with ack
- **Escape** — quit
