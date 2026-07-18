#pragma once

#include <cstdint>

namespace Engine
{

    struct NetworkPeerIds
    {
        static constexpr uint32_t Invalid = 0;
        static constexpr uint32_t Local = 1;

        static uint32_t FromEnetPeer(uint32_t enetPeerId)
        {
            return enetPeerId + Local + 1;
        }
    };
}
