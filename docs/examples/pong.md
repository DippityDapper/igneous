# pong

**Target:** `example_pong`  
**Feature:** Sprite, AudioStream, InputMap, Scene loop, Input, Time

Simple Pong with sprite rendering and sound effects. Demonstrates screen-space sprites for paddles and ball, plus one-shot audio on paddle hits, wall bounces, and scoring.

## Run

```bash
./build/examples/pong/example_pong
```

## Controls

Keyboard bindings live in `assets/input_map.json`. **Gamepads are assigned automatically** by how many are connected:

| Devices | Left paddle | Right paddle |
|---------|-------------|--------------|
| Keyboard only | W/S | CPU (default) or arrows (Tab toggles 2P) |
| Keyboard + 1 gamepad | W/S | Gamepad 1 |
| 2 gamepads | Gamepad 1 | Gamepad 2 |

The ImGui panel shows the current assignment. Use **Reload input map** after editing JSON.

- **toggle_mode** — vs CPU / keyboard 2P when no gamepads are connected
- **restart** — reset score
- **quit** — exit

## Assets

See `examples/pong/assets/README.md` for the full file list:

- `assets/sprites/court.png` — 800×480 background
- `assets/sprites/paddle_left.png` — 12×72 left paddle
- `assets/sprites/paddle_right.png` — 12×72 right paddle
- `assets/sprites/ball.png` — 10×10 ball
- `assets/sounds/paddle_hit.wav` — paddle collision
- `assets/sounds/wall_bounce.wav` — top/bottom wall
- `assets/sounds/score.wav` — point scored
- `assets/input_map.json` — keyboard bindings
