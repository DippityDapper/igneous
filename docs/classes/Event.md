# Event

**Header:** `include/igneous/engine/Event.hpp`  
**Namespace:** `Engine`

## Overview

Template type-safe publish–subscribe signal. `Event<Ret, Args...>` stores callbacks and invokes them in connection order via `Emit()`.

Only `void` return types are supported (`static_assert` rejects other `Ret` values). Capture results in the callback closure if needed.

## Nested Types

- **`Connection`** — Handle returned by `Connect()`; used with `Disconnect()`
- **`CallbackEntry`** — Internal storage (private)

## Connect Overloads

- `std::function` and generic callables
- Member functions (raw pointer or `weak_ptr`)
- Const member functions

## Related

Used throughout the engine for decoupled callbacks (e.g. networking, identity).
