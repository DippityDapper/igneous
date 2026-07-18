# InputBinding

**Header:** `include/igneous/input/InputBinding.hpp`  
**Implementation:** `src/input/InputBinding.cpp`  
**Namespace:** `Engine`

## Overview

Binding types that map hardware input to action values:

| Type | Class |
|------|-------|
| Keyboard | `KeyBinding` |
| Mouse | `MouseButtonBinding` |
| Gamepad button | `GamepadButtonBinding` |
| Gamepad axis | `GamepadAxisBinding` |

Factory helpers: `MakeKeyBinding`, `MakeMouseButtonBinding`, `MakeGamepadButtonBinding`, `MakeGamepadAxisBinding`.

## Related

- [InputAction](InputAction.md)
- [GamepadAxis](GamepadAxis.md)
