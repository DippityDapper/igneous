# ThreadPool

**Header:** `include/igneous/engine/ThreadPool.hpp`  
**Namespace:** `Engine`

## Overview

Fixed-size worker thread pool with a task queue. Tasks are submitted via `Enqueue()` and return `std::future` results.

## API

| Method | Description |
|--------|-------------|
| `ThreadPool(numThreads)` | Default: `hardware_concurrency()` workers |
| `Enqueue(F, args...)` | Schedule work; returns `std::future` |
| `~ThreadPool()` | Signals stop and joins all workers |
