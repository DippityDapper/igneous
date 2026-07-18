# InputMapLoader

**Header:** `include/igneous/input/InputMapLoader.hpp`  
**Implementation:** `src/input/InputMapLoader.cpp`  
**Namespace:** `Engine`

## Overview

Load and save input maps as JSON files.

```cpp
Engine::Input::SetInputMap(Engine::InputMapLoader::LoadFromFile("assets/default_input_map.json"));
```

## JSON Format

```json
{
  "id": "default",
  "name": "Default",
  "actions": [
    {
      "name": "move_right",
      "bindings": [
        { "type": "key", "key": "D" },
        { "type": "gamepad_axis", "axis": "left_x_right" }
      ]
    }
  ]
}
```

### Binding types

| type | fields | notes |
|------|--------|-------|
| `key` | `key` | SDL key name (`Space`, `Escape`, `D`, …) |
| `mouse` | `button` | `left`, `right`, `middle`, `x1`, `x2` |
| `gamepad_button` | `button` | SDL name (`a`, `b`, `start`, …) or alias (`south`, `cross`, …) |
| `gamepad_axis` | `axis` | Semantic axis name (`left_x_right`, `left_y_up`, …) |

## API

| Method | Description |
|--------|-------------|
| `LoadFromFile(path)` | Parse JSON file into `shared_ptr<InputMap>` |
| `LoadFromString(json)` | Parse JSON string |
| `SaveToFile(map, path)` | Write map to JSON file |
| `SaveToString(map)` | Serialize map to JSON string |

## Related

- [InputMap](InputMap.md)
- [InputMapQuery](InputMapQuery.md)
