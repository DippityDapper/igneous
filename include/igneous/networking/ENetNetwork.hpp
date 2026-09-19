#pragma once

#include "igd_desc_parse.h"
#include "miniupnpc.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "enet/enet.h"

#include "igneous/engine/ThreadSafeQueue.hpp"
#include "igneous/networking/ClientConnectionWatchdog.hpp"
#include "igneous/networking/EnetOutboundOperation.hpp"
#include "igneous/networking/ILoopbackNetwork.hpp"
#include "igneous/networking/IPeerDisconnectNetwork.hpp"
#include "igneous/networking/NetworkInterface.hpp"
#include "igneous/networking/ServerPeerActivityTracker.hpp"

namespace Engine
{

    class ENetNetwork : public NetworkInterface, public ILoopbackNetwork, public IPeerDisconnectNetwork
    {
      private:
        NetworkLoopbackLink loopback{};
        ClientConnectionWatchdog clientWatchdog{};
        ServerPeerActivityTracker serverPeers{};

        bool isServer = false;
        std::atomic<bool> running = false;
        bool connecting = false;

        ENetHost* host = nullptr;
        ENetPeer* serverPeer = nullptr;
        std::unordered_map<uint32_t, ENetPeer*> peerLookup{};

        static constexpr uint32_t EnetPeerPingIntervalMs = 500;

        std::thread networkThread;
        std::thread upnpThread;

        Engine::ThreadSafeQueue<NetworkMessage> fromNetwork{};
        Engine::ThreadSafeQueue<EnetOutboundOperation> outbound{};

        int port = -1;

        UPNPDev* upnpDevList = nullptr;
        UPNPUrls upnpUrls{};
        IGDdatas upnpData{};
        bool upnpMapped = false;

      private:
        void NetworkLoop();
        void ProcessOutbound();
        void ServerPing(double now);
        bool HandleClientTimeout(double now);
        void DisconnectStalePeers(double now);
        void DisconnectStalePeer(uint32_t peerId);
        void ConfigureServerPeer(ENetPeer* peer);
        void HandleServerEvent(const ENetEvent& event);
        void HandleClientEvent(const ENetEvent& event);
        void SendToServerOnNetworkThread(const std::vector<uint8_t>& data, TransportType flags);
        void SendToClientOnNetworkThread(uint32_t peerId, const std::vector<uint8_t>& data, TransportType flags);
        void DisconnectPeerOnNetworkThread(uint32_t peerId);
        void GracefulClientDisconnectOnNetworkThread();
        void MapPort(int listenPort);

        static double GetTime();
        static enet_uint32 ToEnetFlags(TransportType flags);

      public:
        ENetNetwork() = default;

        ~ENetNetwork() override;

        void Connect(int listenPort, int peerCount, bool localOnly, bool enableUpnp = false);

        void Connect(int listenPort, const std::string& ip);

        void Connect();

        NetworkLoopbackLink& GetLoopback() override
        {
            return loopback;
        }

        ClientConnectionWatchdog& GetWatchdog()
        {
            return clientWatchdog;
        }

        ServerPeerActivityTracker& GetServerPeers()
        {
            return serverPeers;
        }

        void SendToServer(const std::vector<uint8_t>& data, TransportType flags) override;

        void SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, TransportType flags) override;

        void DisconnectPeer(uint32_t peerId) override;

        void Poll() override;

        bool Connected() override;

        bool IsUpnpActive() const
        {
            return upnpThread.joinable();
        }

        void Clean() override;
    };
}
