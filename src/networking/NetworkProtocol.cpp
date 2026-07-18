#include "igneous/networking/NetworkProtocol.hpp"

#include "igneous/networking/Serializer.hpp"

namespace Engine::NetworkProtocol
{
    Serializer& WritePacketHeader(Serializer& serializer, uint16_t packetType)
    {
        return serializer.Write(packetType);
    }

    uint16_t ReadPacketHeader(const std::vector<uint8_t>& data)
    {
        Deserializer deserializer(data, 0);
        return deserializer.ReadUShort();
    }
}
