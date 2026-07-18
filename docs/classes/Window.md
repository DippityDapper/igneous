# Window

**Header:** `include/igneous/rendering/Window.hpp`  
**Implementation:** `src/rendering/Window.cpp`  
**Namespace:** `Engine`

## Overview

Static SDL window management. Holds viewport dimensions updated on resize.

## API

| Method / Member | Description |
|-----------------|-------------|
| `Init(w, h)` | Create resizable window (title: "client") |
| `Clean()` | Destroy window |
| `GetWindow()` | Raw `SDL_Window*` |
| `viewport` | Current size as `Vec2<int>` (width, height) |
