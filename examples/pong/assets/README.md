# Pong example assets

Drop PNG sprites and WAV sounds here. CMake copies this folder next to the executable at build time.

## Sprites (`sprites/`)

| File | Size | Notes |
|------|------|-------|
| `court.png` | 800×480 px | Full background; centered on the window. |
| `paddle_left.png` | 12×72 px | Left player paddle (blue side). |
| `paddle_right.png` | 12×72 px | Right player / CPU paddle (orange side). |
| `ball.png` | 10×10 px | Ball sprite, centered on the physics hitbox. |

PNG with transparency recommended for paddles and ball.

## Sounds (`sounds/`)

| File | Notes |
|------|-------|
| `paddle_hit.wav` | Plays when the ball hits a paddle. |
| `wall_bounce.wav` | Plays when the ball bounces off the top or bottom wall. |
| `score.wav` | Plays when either side scores. |

Short one-shot WAV files work best.

## Input (`input_map.json`)

Bindings for keyboard (see `input_map.json`). Gamepads are assigned automatically by connection order:

| Connected devices | Left paddle | Right paddle |
|-------------------|-------------|--------------|
| None | Keyboard W/S | CPU (Tab toggles arrow-key 2P) |
| None (2P mode) | Keyboard W/S | Keyboard arrows |
| 1 gamepad | Keyboard W/S | Gamepad 1 left stick |
| 2+ gamepads | Gamepad 1 left stick | Gamepad 2 left stick |

Edit keyboard keys in JSON and click **Reload input map** in the example.
