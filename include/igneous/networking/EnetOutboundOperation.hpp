// Doc: docs/classes/EnetOutboundOperation.md
#pragma once

#include <cstdint>
#include <vector>

#include "igneous/networking/NetworkEvents.hpp"

namespace Engine
{

    enum class EnetOutboundKind
    {
        SendToServer,
        SendToClient,
        DisconnectPeer,
        GracefulClientDisconnect
    };

    struct EnetOutboundOperation
    {
        EnetOutboundKind kind = EnetOutboundKind::SendToServer;
        uint32_t peerId = 0;
        std::vector<uint8_t> data{};
        TransportType transport = TransportType::Reliable;
    };
}
