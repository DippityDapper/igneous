# input_actions

**Target:** `example_input_actions`  
**Feature:** Input maps and actions with mixed device bindings

Each action combines keyboard, mouse, and gamepad bindings. Load from JSON or build the same map in code and toggle at runtime.

## Assets

- `assets/flexible_input_map.json` — mixed bindings (`fire` = F + mouse left + RT)

## Controls

- **WASD / arrows / left stick** — movement actions composed into an axis
- **F / mouse left / RT** — `fire` action
- **Space / gamepad A** — `jump` action
- **Escape** — quit
- **Toggle JSON / code map** — switch input map source

## Related

- [InputMap](../classes/InputMap.md)
- [InputAction](../classes/InputAction.md)
- [InputMapLoader](../classes/InputMapLoader.md)
- [InputMapQuery](../classes/InputMapQuery.md)
