# networking_steam

**Target:** `example_networking_steam`  
**Feature:** SteamNetwork P2P listen-server with loopback client

Requires Steamworks (`-DIGNEOUS_STEAM=ON` and SDK in `libs/steamworks/`). The **Steam client must be running**. A `steam_appid.txt` (Spacewar `480`) is copied next to the executable for offline init.

## Run

```bash
cmake -B build -DIGNEOUS_STEAM=ON
cmake --build build --target example_networking_steam
./build/examples/networking_steam/example_networking_steam
```

## Controls

- **Client -> Server** — send packet via loopback (when Steam enabled)
- **Escape** — quit
