# SceneManager

**Header:** `include/igneous/scenes/SceneManager.hpp`  
**Implementation:** `src/scenes/SceneManager.cpp`  
**Namespace:** `Engine`

## Overview

Global accessor for the single `SceneRoot` instance.

## API

| Method | Description |
|--------|-------------|
| `Init()` | Create default SceneRoot |
| `SetSceneRoot(root)` | Replace with custom root |
| `RemoveSceneRoot()` | Clean and clear root |
| `GetSceneRoot()` | Current root pointer |
