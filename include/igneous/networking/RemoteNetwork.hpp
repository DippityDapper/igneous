#pragma once

#include "igd_desc_parse.h"
#include "miniupnpc.h"

#include <cstdint>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "enet/enet.h"
#include "enet/types.h"

#include "igneous/engine/ThreadSafeQueue.hpp"
#include "igneous/networking/NetworkInterface.hpp"

namespace Engine
{
    /**
     * @class RemoteNetwork
     * @brief ENet-based networking implementation.
     *
     * RemoteNetwork provides real network communication using ENet and runs
     * its networking logic on a dedicated thread. It supports both server
     * and client modes.
     *
     * Network events are translated into NetworkMessage objects and queued
     * for processing via Poll().
     */
    class RemoteNetwork : public NetworkInterface
    {
      private:
        bool _isServer = false;
        bool _running = false;
        bool _connecting = false;

        ENetHost* _host = nullptr;
        ENetPeer* _serverPeer = nullptr;

        std::unordered_map<uint32_t, ENetPeer*> _peerLookup;
        std::unordered_map<ENetPeer*, uint32_t> _peerIdLookup;
        uint32_t _nextPeerId = 1;

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

      private:
        void NetworkLoop();
        void ServerPing(double now);
        bool ClientTimeout(double now);

        double GetTime() const;

      public:
        /**
         * @brief Constructs a server network.
         *
         * @param port Port to bind to.
         * @param peerCount Maximum number of connected peers.
         * @param localOnly Whether to bind only to localhost.
         */
        RemoteNetwork(int port, int peerCount, bool localOnly);

        /**
         * @brief Constructs a client network.
         *
         * @param port Server port.
         * @param ip Server IP address.
         */
        RemoteNetwork(int port, const std::string& ip);

        /**
         * @brief Constructs a loopback-only client (no real connection).
         */
        RemoteNetwork();

        /**
         * @brief Destructor. Calls Clean().
         */
        ~RemoteNetwork() override;

        void HandleServerEvent(const ENetEvent& event);
        void HandleClientEvent(const ENetEvent& event);

        void SendToServer(const std::vector<uint8_t>& data, enet_uint32 flags) override;
        void SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, enet_uint32 flags) override;
        void Poll() override;
        bool Connected() override;
        void Clean() override;
    };
}