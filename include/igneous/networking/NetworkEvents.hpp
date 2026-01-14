#pragma once

#include <vector>

#include "enet/enet.h"
#include "enet/types.h"

namespace Engine
{
    enum class NetworkEventType : uint8_t
    {
        Message = 0,
        ClientConnected = 1,
        ClientDisconnected = 2,
        ServerDisconnected = 3
    };

    struct NetworkMessage
    {
        NetworkEventType type = NetworkEventType::Message;
        ENetPeer* peer = nullptr;
        std::vector<uint8_t> data{};
        enet_uint32 flags = 0;
    };
}
