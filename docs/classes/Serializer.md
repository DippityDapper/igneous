# Serializer / Deserializer

**Header:** `include/igneous/networking/Serializer.hpp`  
**Namespace:** `Engine`

## Serializer

Builds an owned byte buffer with chained `Write()` calls.

### Write Overloads

Primitives: `bool`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `float`, `double`

- `Write(string)` — uint32 length + UTF-8 bytes
- `Write(vector<uint8_t>)` — int32 length + raw bytes
- `Write<T>(vector, writeItem)` — int32 count + per-element callback

`GetBytes()` returns the buffer copy.

## Deserializer

Reads from a const buffer starting at offset (default 2).

### Read Methods

`ReadBool`, `ReadByte`, `ReadShort`, `ReadUShort`, `ReadInt`, `ReadUInt`, `ReadLong`, `ReadULong`, `ReadFloat`, `ReadDouble`, `ReadString`, `ReadBytes`, `ReadList<T>(readItem)`

Throws `std::out_of_range` on buffer underrun.
