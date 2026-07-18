// Doc: docs/classes/SteamNetwork.md
#pragma once

#include "igneous/networking/NetworkLoopbackLink.hpp"
#include "igneous/networking/ClientConnectionWatchdog.hpp"
#include "igneous/networking/ILoopbackNetwork.hpp"
#include "igneous/networking/NetworkInterface.hpp"
#include "igneous/networking/ServerPeerActivityTracker.hpp"

#ifdef IGNEOUS_STEAM_ENABLED
#include <atomic>
#include <cstdint>
#include <thread>
#include <unordered_map>
#include <vector>

#include <steam/isteamnetworkingsockets.h>
#include <steam/steam_api.h>

#include "igneous/engine/ThreadSafeQueue.hpp"
#endif

namespace Engine
{

    class SteamNetwork : public NetworkInterface, public ILoopbackNetwork
    {
      private:
        NetworkLoopbackLink loopback{};
        ClientConnectionWatchdog clientWatchdog{};

#ifdef IGNEOUS_STEAM_ENABLED
        ServerPeerActivityTracker serverPeers{};
        bool isServer = false;
        std::atomic<bool> running = false;
        std::atomic<bool> connected = false;

        std::thread networkThread;
        Engine::ThreadSafeQueue<NetworkMessage> fromNetwork{};

        std::unordered_map<uint32_t, HSteamNetConnection> connections{};

        HSteamNetConnection serverConnection = k_HSteamNetConnection_Invalid;
        HSteamListenSocket listenSocket = k_HSteamListenSocket_Invalid;
        HSteamNetPollGroup pollGroup = k_HSteamNetPollGroup_Invalid;

        STEAM_CALLBACK(SteamNetwork, OnConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);

        void NetworkLoop();
        void ServerPing(double now);
        bool HandleClientTimeout(double now);
        void DisconnectStaleConnections(double now);
        void DisconnectStaleConnection(uint32_t connId);
        void ReceiveMessages();
        void SendMessage(HSteamNetConnection conn, const std::vector<uint8_t>& data, TransportType flags);

        static double GetTime();
        static int ToSteamFlags(TransportType flags);
#endif

      public:
        SteamNetwork() = default;

        ~SteamNetwork() override;

        void Connect();

        void Connect(uint64_t hostSteamId);

        NetworkLoopbackLink& GetLoopback() override
        {
            return loopback;
        }

        ClientConnectionWatchdog& GetWatchdog()
        {
            return clientWatchdog;
        }

#ifdef IGNEOUS_STEAM_ENABLED
        ServerPeerActivityTracker& GetServerPeers()
        {
            return serverPeers;
        }
#endif

        void SendToServer(const std::vector<uint8_t>& data, TransportType flags) override;

        void SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, TransportType flags) override;

        void Poll() override;

        bool Connected() override;

        void Clean() override;
    };
}
