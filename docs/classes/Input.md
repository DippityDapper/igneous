# Input

**Header:** `include/igneous/input/Input.hpp`  
**Implementation:** `src/input/Input.cpp`  
**Namespace:** `Engine`

## Overview

Static singleton input system for keyboard, mouse, gamepad, input layers, and action maps. Tracks per-frame pressed/just-pressed/released state and handled flags.

## Initialization

`Input::Init()` — called by Engine; sets up gamepads and default layer.

## Input Layers

| Method | Description |
|--------|-------------|
| `AddInputLayer(name, priority)` | Register layer (higher priority first) |
| `RemoveInputLayer(name)` | Remove layer |
| `GetInputLayers()` | All layers sorted by priority descending |

Layer name constants: `Input::GameplayLayer`, `Input::UILayer`, `Input::MenuLayer`.

## Action Maps

Games define bindings in code or load them from JSON via [InputMapLoader](InputMapLoader.md).

| Method | Description |
|--------|-------------|
| `SetInputMap(map)` | Set the active input map |
| `GetInputMap()` | Current map, or `nullptr` |
| `GetInputAction(name)` | Lookup action |
| `GetAction(name)` | Action value in `[0, 1]` |
| `IsActionPressed`, `IsActionJustPressed`, `IsActionJustReleased` | Action state |
| `GetAxis(positive, negative)` | Composed axis from two actions |

## Keyboard / Mouse

| Method | Description |
|--------|-------------|
| `IsKeyDown`, `IsKeyJustPressed`, `IsKeyJustReleased` | Key state queries |
| `IsMouseButtonDown`, `IsButtonJustPressed`, `IsButtonJustReleased` | Mouse button state |
| `GetMouseScreenPosition`, `GetMouseVelocity`, `GetMouseWheelVelocity` | Pointer data |
| `HandleKey`, `HandleButton`, `IsKeyHandled`, `IsButtonHandled` | Consumption tracking |
| `HandleAllInputs(...)` | Mark all inputs handled except optional exceptions |

## Gamepad

| Method | Description |
|--------|-------------|
| `GetConnectedGamepads()` | List of joystick IDs |
| `IsGamepadButtonDown/JustPressed/JustReleased` | Button state |
| `GetGamepadAxis`, `GetGamepadStick` | Raw SDL axis values |
| `GetSemanticGamepadAxis`, `GetSemanticGamepadStick` | Directional stick halves with deadzone |
| `RumbleGamepad`, `RumbleGamepadTriggers` | Haptic feedback |

Semantic axes use [GamepadAxis](GamepadAxis.md) (`left_x_right`, `left_y_up`, etc.).

## Frame Cycle

Call `ResetEvents()` at frame start, then `HandleEvent(SDL_Event&)` for each SDL event.

Window resize is handled by [Window](Window.md) (`Window::OnResize`); `IsWindowResized()` delegates to `Window::WasResized()`.

## Test hooks

| Method | Description |
|--------|-------------|
| `ResetForTests()` | Clears layers, devices, events, and action map |
| `RestoreBaseline()` | Re-adds the `_default` layer after shutdown or full reset |

## Related

- [InputMap](InputMap.md)
- [InputAction](InputAction.md)
- [InputBinding](InputBinding.md)
- [InputMapQuery](InputMapQuery.md)
- [InputMapLoader](InputMapLoader.md)
- [InputLayer](InputLayer.md)
- [InputEvent](InputEvent.md)
