# GamepadAxis

**Header:** `include/igneous/input/GamepadAxis.hpp`  
**Implementation:** `src/input/GamepadAxis.cpp`  
**Namespace:** `Engine`

## Overview

Semantic gamepad axis identifiers used by action bindings and `Input::GetSemanticGamepadAxis`.

| Value | Meaning |
|-------|---------|
| `LeftXRight`, `LeftXLeft` | Left stick horizontal halves |
| `LeftYUp`, `LeftYDown` | Left stick vertical halves |
| `RightXRight`, `RightXLeft` | Right stick horizontal halves |
| `RightYUp`, `RightYDown` | Right stick vertical halves |
| `TriggerLeft`, `TriggerRight` | Analog triggers |

String helpers: `GamepadAxisToString`, `GamepadAxisFromString`.

## Related

- [Input](Input.md)
- [InputBinding](InputBinding.md)
