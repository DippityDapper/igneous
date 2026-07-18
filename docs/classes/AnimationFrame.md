# AnimationFrame

**Header:** `include/igneous/resources/AnimationFrame.hpp`  
**Namespace:** `Engine`

## Overview

Single animation frame: shared texture plus atlas rectangle for sub-sprite rendering.

## Members

| Member | Description |
|--------|-------------|
| `texture` | `shared_ptr<SDL_Texture>` |
| `atlasW`, `atlasH` | Source rectangle size |
| `atlasX`, `atlasY` | Source rectangle offset |
