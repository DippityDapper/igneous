#pragma once

#include <queue>

#include "igneous/networking/NetworkInterface.hpp"

namespace Engine
{
    class LocalNetwork : public NetworkInterface
    {
    private:
        std::queue<NetworkMessage> incomingMessages;
        LocalNetwork* networkPeer = nullptr;

    public:
        ~LocalNetwork() override = default;
        void SetPeer(LocalNetwork* peer);
        void SendToServer(const std::vector<uint8_t>& data, enet_uint32 flags) override;
        void SendToClient(ENetPeer* peer, const std::vector<uint8_t>& data, enet_uint32 flags) override;
        void Poll() override;
        bool Connected() override;
    };
}
