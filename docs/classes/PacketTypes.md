# PacketTypes

**Header:** `include/igneous/networking/PacketTypes.hpp`  
**Namespace:** `Engine`

## Overview

`PacketType` enumerates wire message identifiers written as the first two bytes of each payload (see [Serializer wire format](Serializer.md#wire-format)). Numeric values are fixed for protocol v0.

## Values

Connection lifecycle, lobby, player data, entity sync, equipment, chat, and input packet types.

`ConnectionRequest_` and `DisconnectionRequest_` are reserved legacy wire slots from a prior protocol revision. They are not used by current handlers but must keep their numeric values for wire compatibility.

## Related

- [PacketRouter](PacketRouter.md), [Serializer](Serializer.md)
