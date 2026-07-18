# Vec3

**Header:** `include/igneous/engine/Vec3.hpp`  
**Namespace:** `Engine`

## Overview

Generic 3D vector template (`Vec3<T>`) where `T` is an arithmetic type. Same operator and math surface as `Vec2`, plus 3D cross product.

## Notable Methods

| Method | Description |
|--------|-------------|
| `Normalized()` | Unit vector |
| `Cross(rhs)` | 3D cross product → `Vec3<float>` |
| `DistanceTo`, `Dot`, `Proj`, `Comp` | Standard vector operations |
