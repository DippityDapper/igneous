# ThreadSafeQueue

**Header:** `include/igneous/engine/ThreadSafeQueue.hpp`  
**Namespace:** *(global)*

## Overview

Mutex-protected FIFO queue for cross-thread message passing. Used by networking backends to deliver events to the main thread.

## API

| Method | Description |
|--------|-------------|
| `Push(value)` | Enqueue a copy |
| `Pop()` | Dequeue or return `std::nullopt` if empty |
| `Empty()` | Thread-safe empty check |
