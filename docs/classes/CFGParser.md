# CFGParser

**Header:** `include/igneous/engine/CFGParser.hpp`  
**Implementation:** `src/engine/CFGParser.cpp`  
**Namespace:** `Engine`

## Overview

Static key-value configuration loader. Supports multiple named configs in memory, `#` comments, and typed getters/setters.

## Format

```
# comment
world_size=small
debug_mode=true
```

## API

| Method | Description |
|--------|-------------|
| `LoadConfig(path, name)` | Parse file into named config |
| `SaveConfig(path, name)` | Write config to disk |
| `GetString/Int/UInt32/UInt16/Bool` | Typed reads (throws on missing key). `GetUInt16` returns `uint16_t` and clamps out-of-range values with a logged warning. |
| `WriteString/Int/UInt32/UInt16/Bool` | Typed writes (in memory until save) |
