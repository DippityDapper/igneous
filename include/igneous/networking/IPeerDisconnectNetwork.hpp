#pragma once

#include <cstdint>

namespace Engine
{

    class IPeerDisconnectNetwork
    {
      public:
        virtual ~IPeerDisconnectNetwork() = default;

        virtual void DisconnectPeer(uint32_t peerId) = 0;
    };
}
