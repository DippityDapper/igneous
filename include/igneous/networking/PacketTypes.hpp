// Doc: docs/classes/PacketTypes.md
#pragma once

#include <cstdint>

namespace Engine
{

    /// Wire message identifiers (first two bytes of each payload). Values are fixed for protocol v0.
    enum class PacketType : uint16_t
    {
        // Connection lifecycle
        ConnectionRequest = 0,
        ConnectionRequest_ = 1, ///< Reserved wire slot from legacy protocol; do not reuse.
        ConnectionAccepted = 2,
        ConnectionRejected = 3,
        DisconnectNoticePacket = 4,
        ClientConnected = 5,

        ServerIdRequest = 6,
        ServerIdPacket = 7,
        ServerDisconnected = 8,

        DisconnectionRequest = 9,
        DisconnectionRequest_ = 10, ///< Reserved wire slot from legacy protocol; do not reuse.
        DisconnectionAcknowledged = 11,
        ClientDisconnected = 12,

        // Lobby
        LobbyPacket = 13,
        LobbyRequest = 14,
        LobbyAcknowledged = 15,

        // Player sync
        PlayerDataPacket = 16,
        PlayerLeavePacket = 17,

        InputPacket = 18,

        // Entity sync
        EntityPacket = 19,
        EntityRemovePacket = 20,
        EntitySyncPacket = 21,

        EquipmentPacket = 22,

        ChatMessagePacket = 23
    };
}
