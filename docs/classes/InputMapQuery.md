# InputMapQuery

**Header:** `include/igneous/input/InputMapQuery.hpp`  
**Namespace:** `Engine`

## Overview

Helpers for composing analog axes from opposing actions.

```cpp
float horizontal = Engine::InputMapQuery::GetAxis("move_right", "move_left");
float vertical = Engine::Input::GetAxis("move_down", "move_up");
```

`GetAxisFromValues(positive, negative)` returns `positive - negative`.

## Related

- [Input](Input.md)
- [InputMap](InputMap.md)
