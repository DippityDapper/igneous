# Serializer / Deserializer

**Header:** `include/igneous/networking/Serializer.hpp`  
**Protocol helpers:** `include/igneous/networking/NetworkProtocol.hpp`  
**Namespace:** `Engine`

## Wire format

Routed network messages use a fixed 2-byte prefix followed by the payload:

| Offset | Size | Field |
|--------|------|-------|
| 0 | 2 | `PacketType` as `uint16_t` (native endian) |
| 2 | … | Payload bytes (`Serializer` output after the header) |

- `NetworkProtocol::HeaderSize` is `sizeof(uint16_t)` (2).
- `NetworkProtocol::WritePacketHeader(serializer, packetType)` writes the prefix.
- `NetworkProtocol::ReadPacketHeader(data)` reads the prefix from a full packet buffer.
- `PacketRouter::DispatchMessage` uses the prefix to select handlers; handlers receive the **full** buffer and should construct `Deserializer(data)` with the default offset so the header is skipped automatically.

See also [NetworkEvents](NetworkEvents.md) for how payloads arrive on `NetworkMessage::data`.

## Endianness

`Serializer` / `Deserializer` write and read arithmetic types with **native endianness** via `reinterpret_cast`. Messages are **not** portable across big-endian / little-endian peers without an explicit conversion layer. There is no protocol version field in the header today.

## Serializer

Builds an owned byte buffer with chained `Write()` calls.

### Write Overloads

Primitives: `bool`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `float`, `double`

- `Write(string)` — uint32 length + UTF-8 bytes
- `Write(vector<uint8_t>)` — int32 length + raw bytes
- `Write<T>(vector, writeItem)` — int32 count + per-element callback

`GetBytes()` returns the buffer copy.

## Deserializer

Reads from a const buffer. The default start offset is `NetworkProtocol::HeaderSize`, skipping the packet-type prefix on routed messages. Pass `0` when reading a buffer with no header (e.g. unit tests of raw primitive encoding).

### Read Methods

`ReadBool`, `ReadByte`, `ReadShort`, `ReadUShort`, `ReadInt`, `ReadUInt`, `ReadLong`, `ReadULong`, `ReadFloat`, `ReadDouble`, `ReadString`, `ReadBytes`, `ReadList<T>(readItem)`

Throws `std::out_of_range` on buffer underrun.
