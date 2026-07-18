# TextureDeleter

**Header:** `include/igneous/resources/ResourceManager.hpp`  
**Namespace:** `Engine`

## Overview

Function object for `shared_ptr<SDL_Texture>` custom deleter. Calls `SDL_DestroyTexture` when the last reference is released.

Used internally by `ResourceManager` texture caching.
