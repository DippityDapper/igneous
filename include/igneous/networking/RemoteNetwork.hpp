#pragma once

#include <cstdint>
#include <string>
#include <thread>
#include <vector>

#include "enet/enet.h"
#include "enet/types.h"

#include "igneous/engine/ThreadSafeQueue.hpp"
#include "igneous/networking/NetworkInterface.hpp"

namespace Engine
{
    class RemoteNetwork : public NetworkInterface
    {
    private:
        bool isServer = false;
        bool running = false;

        ENetHost* host = nullptr;
        ENetPeer* serverPeer = nullptr;

        std::thread networkThread;
        ThreadSafeQueue<NetworkMessage> fromNetwork{};

    private:
        void NetworkLoop();

    public:
        RemoteNetwork(int port, int peerCount, bool localOnly);
        RemoteNetwork(int port, const std::string& ip);
        ~RemoteNetwork() override;

        void SendToServer(const std::vector<uint8_t>& data, enet_uint32 flags) override;
        void SendToClient(ENetPeer* peer, const std::vector<uint8_t>&, enet_uint32 flags) override;
        void Poll() override;
        bool Connected() override;

        void HandleServerEvent(const ENetEvent& event);
        void HandleClientEvent(const ENetEvent& event);

        void SendPacket(ENetPeer* peer, std::vector<uint8_t> data, enet_uint32 packetType);
    };
}
