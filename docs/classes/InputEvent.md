# InputEvent

**Header:** `include/igneous/input/InputEvent.hpp`  
**Namespace:** `Engine`

## Overview

Per-input state tracked by the Input system for keys, mouse buttons, and gamepad buttons.

## Members

| Member | Description |
|--------|-------------|
| `pressed` | Currently held down |
| `pressedLastFrame` | Previous frame state (for edge detection) |
| `handled` | Consumed by a higher-priority handler |

Do not modify directly; managed by `Input`.
