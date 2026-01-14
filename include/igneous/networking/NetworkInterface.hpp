#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "enet/enet.h"

#include "igneous/networking/NetworkEvents.hpp"

namespace Engine
{
    class NetworkInterface
    {
    public:
        std::function<void(const NetworkMessage&)> onMessageReceived;

    public:
        virtual ~NetworkInterface() = default;
        virtual void SendToServer(const std::vector<uint8_t>& data, enet_uint32 flags) = 0;
        virtual void SendToClient(ENetPeer* peer, const std::vector<uint8_t>& data, enet_uint32 flags) = 0;
        virtual void Poll() = 0;
        virtual bool Connected() = 0;
    };
}
