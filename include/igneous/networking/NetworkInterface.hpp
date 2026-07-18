// Doc: docs/classes/NetworkInterface.md
#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "igneous/networking/NetworkEvents.hpp"

namespace Engine
{

    class NetworkInterface
    {
      public:
        std::function<void(const NetworkMessage&)> onMessageReceived;

      public:
        virtual ~NetworkInterface() = default;

        virtual void SendToServer(const std::vector<uint8_t>& data, TransportType flags) = 0;

        virtual void SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, TransportType flags) = 0;

        virtual void Poll() = 0;

        virtual bool Connected() = 0;

        virtual void Clean() = 0;
    };
}
