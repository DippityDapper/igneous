#pragma once

#include <cstdint>

namespace Engine
{

    enum class PacketType : uint16_t
    {

        ConnectionRequest = 0,
        ConnectionRequest_ = 1,
        ConnectionAccepted = 2,
        ConnectionRejected = 3,
        DisconnectNoticePacket = 4,
        ClientConnected = 5,

        ServerIdRequest = 6,
        ServerIdPacket = 7,
        ServerDisconnected = 8,

        DisconnectionRequest = 9,
        DisconnectionRequest_ = 10,
        DisconnectionAcknowledged = 11,
        ClientDisconnected = 12,

        LobbyPacket = 13,
        LobbyRequest = 14,
        LobbyAcknowledged = 15,

        PlayerDataPacket = 16,
        PlayerLeavePacket = 17,

        InputPacket = 18,

        EntityPacket = 19,
        EntityRemovePacket = 20,
        EntitySyncPacket = 21,

        EquipmentPacket = 22,

        ChatMessagePacket = 23
    };
}
