#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "igneous/networking/PacketTypes.hpp"
#include "igneous/networking/Serializer.hpp"

namespace Engine
{

    class PacketRouter
    {
      public:
        using Handler = std::function<void(uint32_t peerId, const std::vector<uint8_t>& data)>;

        size_t Connect(PacketType type, Handler handler);

        void Dispatch(PacketType type, uint32_t peerId, const std::vector<uint8_t>& data);

        void DispatchMessage(uint32_t peerId, const std::vector<uint8_t>& data);

        void Disconnect(PacketType type, size_t handlerId);

        void Clear();

      private:
        struct HandlerEntry
        {
            size_t id = 0;
            Handler handler{};
        };

        std::unordered_map<PacketType, std::vector<HandlerEntry>> handlers{};
        size_t nextHandlerId = 1;
    };
}
