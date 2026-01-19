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
     *
     * @see Engine::NetworkManager::CreateServer() For creating a RemoteNetwork
     * server.
     * @see Engine::NetworkManager::CreateClient() For creating a RemoteNetwork
     * client.
     */
    class RemoteNetwork : public NetworkInterface
    {
      private:
        /**
         * @brief Whether this instance is running as a server.
         */
        bool isServer = false;

        /**
         * @brief Whether the networking thread is active.
         */
        bool running = false;

        /**
         * @brief ENet host instance.
         */
        ENetHost* host = nullptr;

        /**
         * @brief Server peer (client mode only).
         */
        ENetPeer* serverPeer = nullptr;

        /**
         * @brief Background thread handling ENet events.
         */
        std::thread networkThread;

        /**
         * @brief Thread-safe queue of messages received from the network.
         */
        ThreadSafeQueue<NetworkMessage> fromNetwork{};

      private:
        /**
         * @brief Main networking loop executed on a background thread.
         *
         * Polls ENet events and dispatches them to the appropriate handlers.
         */
        void NetworkLoop();

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
         * @brief Destructor.
         *
         * Stops the networking thread and releases ENet resources.
         */
        ~RemoteNetwork() override;

        /**
         * @brief Sends data to the server.
         *
         * Valid only when operating in client mode.
         *
         * @param data Payload to send.
         * @param flags ENet packet flags.
         */
        void SendToServer(const std::vector<uint8_t>& data, enet_uint32 flags) override;

        /**
         * @brief Sends data to a specific client.
         *
         * Valid only when operating in server mode.
         *
         * @param peer Target client peer.
         * @param data Payload to send.
         * @param flags ENet packet flags.
         */
        void SendToClient(ENetPeer* peer, const std::vector<uint8_t>& data, enet_uint32 flags) override;

        /**
         * @brief Processes queued network messages.
         *
         * Delivers messages from the networking thread via onMessageReceived.
         */
        void Poll() override;

        /**
         * @brief Checks whether the network is running.
         *
         * @return true if the networking thread is active.
         */
        bool Connected() override;

        /**
         * @brief Handles ENet events in server mode.
         *
         * @param event ENet event to process.
         */
        void HandleServerEvent(const ENetEvent& event);

        /**
         * @brief Handles ENet events in client mode.
         *
         * @param event ENet event to process.
         */
        void HandleClientEvent(const ENetEvent& event);

        /**
         * @brief Sends a raw packet to a peer.
         *
         * @param peer Target peer.
         * @param data Packet payload.
         * @param packetType ENet packet flags.
         */
        void SendPacket(ENetPeer* peer, std::vector<uint8_t> data, enet_uint32 packetType);
    };
}
