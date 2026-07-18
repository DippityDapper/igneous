# InputMap

**Header:** `include/igneous/input/InputMap.hpp`  
**Implementation:** `src/input/InputMap.cpp`  
**Namespace:** `Engine`

## Overview

Named collection of [InputAction](InputAction.md) entries. Games build maps in code or load them with [InputMapLoader](InputMapLoader.md).

## Code API

```cpp
auto map = std::make_shared<Engine::InputMap>("default", "Default");
map->AddKeyBinding("jump", SDLK_SPACE);
map->AddGamepadButtonBinding("jump", SDL_GAMEPAD_BUTTON_SOUTH);
map->AddGamepadAxisBinding("move_right", Engine::GamepadAxis::LeftXRight);
Engine::Input::SetInputMap(map);
```

## Query API

| Method | Description |
|--------|-------------|
| `FindAction(name)` | Lookup action |
| `AddAction(name)` | Create or return existing action |
| `AddKeyBinding`, `AddMouseBinding`, `AddGamepadButtonBinding`, `AddGamepadAxisBinding` | Convenience helpers |
| `GetValue`, `IsPressed`, `IsJustPressed`, `IsJustReleased` | Action state by name |

## Related

- [Input](Input.md)
- [InputMapLoader](InputMapLoader.md)
