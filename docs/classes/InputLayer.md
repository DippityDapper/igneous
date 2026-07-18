# InputLayer

**Header:** `include/igneous/input/InputLayer.hpp`  
**Implementation:** `src/input/InputLayer.cpp`  
**Namespace:** `Engine`

## Overview

Named input handling layer with priority. Higher-priority layers receive events first during scene and camera input dispatch.

## API

| Method | Description |
|--------|-------------|
| `InputLayer(name, priority)` | Construct layer |
| `Is(layerName)` | Name comparison |
| `GetName()` | Layer name |
| `GetPriority()` | Layer priority |
