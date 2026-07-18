// Doc: docs/classes/Serializer.md
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace Engine
{
    class Serializer;

    namespace NetworkProtocol
    {
        /// Size of the packet-type prefix written before every routed message payload.
        inline constexpr size_t HeaderSize = sizeof(uint16_t);

        Serializer& WritePacketHeader(Serializer& serializer, uint16_t packetType);

        uint16_t ReadPacketHeader(const std::vector<uint8_t>& data);
    }
}
