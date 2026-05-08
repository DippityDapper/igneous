#pragma once

#include "igd_desc_parse.h"
#include "miniupnpc.h"

#include <cstdint>
#include <string>
#include <vector>

#include "igneous/networking/NetworkInterface.hpp"

#ifdef IGNEOUS_STEAM_ENABLED
#include <steam/steam_api.h>
#include <steam/isteamnetworkingsockets.h>

#include <atomic>
#include <thread>
#include <unordered_map>

#include "igneous/engine/ThreadSafeQueue.hpp"
#endif

namespace Engine
{
    /**
     * @class SteamNetwork
     * @brief Steam Networking Sockets-based networking implementation.
     *
     * SteamNetwork provides real network communication using the Steamworks
     * ISteamNetworkingSockets API. It mirrors the structure of RemoteNetwork
     * but uses Steam's relay network instead of direct ENet connections.
     *
     * Network events are translated into NetworkMessage objects and queued
     * for processing via Poll().
     *
     * @note This class requires IGNEOUS_STEAM_ENABLED to be defined at compile
     * time (set automatically when IGNEOUS_STEAM is ON in CMake). Without it,
     * all constructors and methods are stubs that immediately report failure
     * or do nothing.
     */
    class SteamNetwork : public NetworkInterface
    {
#ifdef IGNEOUS_STEAM_ENABLED
      private:
        bool _isServer = false;
        std::atomic<bool> _running = false;
        std::atomic<bool> _connected = false;

        HSteamListenSocket _listenSocket = k_HSteamListenSocket_Invalid;
        HSteamNetConnection _serverConnection = k_HSteamNetConnection_Invalid;
        HSteamNetPollGroup _pollGroup = k_HSteamNetPollGroup_Invalid;

        // Maps Steam connection handle <-> our uint32 peer id
        std::unordered_map<uint32_t, HSteamNetConnection> _connections;

        std::thread _networkThread;
        ThreadSafeQueue<NetworkMessage> _fromNetwork{};

        int _port = -1;

        static const std::vector<uint8_t> PingPacket;
        double _lastPingTime = 0.0;
        double _lastPingSend = 0.0;
        static constexpr double PingTimeout = 5.0;

        UPNPDev* _upnpDevList = nullptr;
        UPNPUrls _upnpUrls{};
        IGDdatas _upnpData{};
        bool _upnpMapped = false;

        STEAM_CALLBACK(SteamNetwork, OnConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);

      private:
        void NetworkLoop();
        void ServerPing(double now);
        bool ClientTimeout(double now);
        void ReceiveMessages();
        void SendMessage(HSteamNetConnection conn, const std::vector<uint8_t>& data, uint32_t flags);

        double GetTime() const;
#endif

      public:
        /**
         * @brief Constructs a server network listening on the given port.
         *
         * @param port UDP port to listen on.
         * @param localOnly If true, bind to 127.0.0.1 only.
         */
        SteamNetwork(int port, bool localOnly);

        /**
         * @brief Constructs a client network connecting to ip:port.
         *
         * @param port Server port.
         * @param ip   Server IP address string.
         */
        SteamNetwork(int port, const std::string& ip);

        /**
         * @brief Constructs a loopback-only client (no real connection).
         */
        SteamNetwork();

        /**
         * @brief Destructor. Calls Clean().
         */
        ~SteamNetwork() override;

        void SendToServer(const std::vector<uint8_t>& data, uint32_t flags) override;
        void SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, uint32_t flags) override;
        void Poll() override;
        bool Connected() override;
        void Clean() override;
    };
}