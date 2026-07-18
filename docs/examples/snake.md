# snake

**Target:** `example_snake`  
**Feature:** Sprite, AudioStream, InputMap, Scene loop, Input, Time

Grid-based Snake with sprite rendering and sound effects. Demonstrates multiple `Sprite` instances (board, head, body segments, food), `ResourceManager::LoadSound`, and fixed-step gameplay.

## Run

```bash
./build/examples/snake/example_snake
```

## Controls

Bindings live in `assets/input_map.json` (keyboard + gamepad). Use **Reload input map** in the ImGui panel after editing the file.

- **turn_*** / **steer_*** — change direction (WASD, arrows, d-pad, left stick)
- **restart** — restart after game over
- **quit** — exit

## Assets

See `examples/snake/assets/README.md` for the full file list:

- `assets/sprites/board.png` — 384×288 playfield
- `assets/sprites/snake_head.png` — 16×16 head (faces right at `rotation = 0`)
- `assets/sprites/snake_body.png` — 16×16 body
- `assets/sprites/food.png` — 16×16 pickup
- `assets/sounds/eat.wav` — food collected
- `assets/sounds/game_over.wav` — collision / wall
- `assets/input_map.json` — keyboard and gamepad bindings
