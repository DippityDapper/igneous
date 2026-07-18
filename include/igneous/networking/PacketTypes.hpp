#pragma once

#include <cstdint>

namespace Engine
{

    enum class PacketType : uint16_t
    {
        ConnectionRequest,
        ConnectionRequest_,
        ConnectionAccepted,
        ConnectionRejected,
        DisconnectNoticePacket,
        ClientConnected,

        ServerIdRequest,
        ServerIdPacket,
        ServerDisconnected,

        DisconnectionRequest,
        DisconnectionRequest_,
        DisconnectionAcknowledged,
        ClientDisconnected,

        LobbyPacket,
        LobbyRequest,
        LobbyAcknowledged,

        PlayerDataPacket,
        PlayerLeavePacket,

        InputPacket,

        EntityPacket,
        EntityRemovePacket,
        EntitySyncPacket,

        EquipmentPacket,

        ChatMessagePacket
    };
}
