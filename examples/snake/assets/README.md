# Snake example assets

Drop PNG sprites and WAV sounds here. CMake copies this folder next to the executable at build time.

## Sprites (`sprites/`)

| File | Size | Notes |
|------|------|-------|
| `board.png` | 384×288 px | Playfield background (24×18 cells at 16 px). Centered on screen. |
| `snake_head.png` | 16×16 px | Snake head; faces **right** at `rotation = 0` |
| `snake_body.png` | 16×16 px | Body segment tile. |
| `food.png` | 16×16 px | Pickup shown on the grid. |

PNG with transparency recommended. Pixel art works best with nearest-neighbor scaling (already set in the example).

## Sounds (`sounds/`)

| File | Notes |
|------|-------|
| `eat.wav` | Plays when the snake collects food. |
| `game_over.wav` | Plays once on collision or wall hit. |

Short one-shot WAV files (similar to `examples/audio/assets/sounds/button/button1.wav`) work well.

## Input (`input_map.json`)

Bindings for keyboard and gamepad. Edit this file and click **Reload input map** in the example to try changes without rebuilding.

| Action | Default bindings |
|--------|------------------|
| `turn_*` | WASD / arrows / d-pad |
| `steer_*` | Left stick (edge-triggered turns) |
| `restart` | R / gamepad West |
| `quit` | Escape / gamepad Back |

See [InputMapLoader](../../docs/classes/InputMapLoader.md) for JSON format.
