#include "igneous/networking/PacketRouter.hpp"

#include "igneous/networking/NetworkProtocol.hpp"

#include <algorithm>

namespace Engine
{
    size_t PacketRouter::Connect(PacketType type, Handler handler)
    {
        const size_t id = nextHandlerId++;
        handlers[type].push_back({id, std::move(handler)});
        return id;
    }

    void PacketRouter::Dispatch(PacketType type, uint32_t peerId, const std::vector<uint8_t>& data)
    {
        auto it = handlers.find(type);
        if (it == handlers.end())
            return;

        const std::vector<HandlerEntry> snapshot = it->second;
        for (const HandlerEntry& entry: snapshot)
            entry.handler(peerId, data);
    }

    void PacketRouter::DispatchMessage(uint32_t peerId, const std::vector<uint8_t>& data)
    {
        const PacketType type = static_cast<PacketType>(NetworkProtocol::ReadPacketHeader(data));
        Dispatch(type, peerId, data);
    }

    void PacketRouter::Disconnect(PacketType type, size_t handlerId)
    {
        auto it = handlers.find(type);
        if (it == handlers.end())
            return;

        auto& list = it->second;
        list.erase(
                std::remove_if(list.begin(), list.end(), [handlerId](const HandlerEntry& entry)
                {
                    return entry.id == handlerId;
                }),
                list.end());

        if (list.empty())
            handlers.erase(it);
    }

    void PacketRouter::Clear()
    {
        handlers.clear();
    }
}
