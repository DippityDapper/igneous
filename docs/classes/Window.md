# Window

**Header:** `include/igneous/rendering/Window.hpp`  
**Implementation:** `src/rendering/Window.cpp`  
**Namespace:** `Engine`

## Overview

Static SDL window management. Holds viewport dimensions updated on resize via `OnResize`.

## API

| Method / Member | Description |
|-----------------|-------------|
| `Init(w, h, title = "client")` | Create resizable window |
| `Clean()` | Destroy window |
| `OnResize(w, h)` | Update `viewport` and mark resized for the current frame |
| `WasResized()` | True if resized since last `ResetFrameState()` |
| `ResetFrameState()` | Clear per-frame resize flag (called by Engine at start of event processing) |
| `GetWindow()` | Raw `SDL_Window*` |
| `viewport` | Current size as `Vec2<int>` (width, height) |
