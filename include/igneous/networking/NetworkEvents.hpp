#pragma once

#include <cstdint>
#include <vector>

namespace Engine
{

    enum class NetworkEventType : uint8_t
    {
        Message,
        Ping,
        ClientConnected,
        ClientDisconnected,
        ServerDisconnected,
        ConnectionSuccess,
        ConnectionFailure
    };

    enum class TransportType : int32_t
    {
        Unreliable,
        Reliable
    };

    struct NetworkMessage
    {
        NetworkEventType type = NetworkEventType::Message;
        uint32_t peerId = 0;
        std::vector<uint8_t> data{};
    };
}
