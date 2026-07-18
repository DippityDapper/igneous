# Database

**Header:** `include/igneous/engine/Database.hpp`  
**Implementation:** `src/engine/Database.cpp`  
**Namespace:** `Engine`

## Overview

Static SQLite helper wrapping SQLiteCpp. Non-instantiable utility for open, execute, and query operations.

## API

| Method | Description |
|--------|-------------|
| `Open(filePath)` | Open or create database; returns `unique_ptr<SQLite::Database>` |
| `Execute(db, query)` | Run non-query SQL |
| `Query(db, query)` | Run SELECT; returns rows as `unordered_map<string,string>` |
