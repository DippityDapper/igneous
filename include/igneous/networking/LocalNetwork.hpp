#pragma once

#include <queue>

#include "igneous/networking/NetworkInterface.hpp"

namespace Engine
{
    /**
     * @class LocalNetwork
     * @brief In-memory networking implementation for local simulation.
     *
     * LocalNetwork simulates networking by directly transferring messages
     * between paired LocalNetwork instances. It is useful for single-process
     * testing, offline play, or deterministic simulations without real sockets.
     *
     * Messages are queued internally and delivered when Poll() is called.
     *
     * @see Engine::NetworkManager::CreateLocalNetwork() For creating a connected
     * server-client LocalNetwork pair.
     */
    class LocalNetwork : public NetworkInterface
    {
      private:
        bool isServer = false;

      public:
        explicit LocalNetwork(bool _isServer);

        /**
         * @brief Destructor.
         */
        ~LocalNetwork() override = default;

        /**
         * @brief Sends data to the server.
         *
         * Pushes a message into the peer's incoming queue with no associated
         * ENet peer.
         *
         * @param data Payload to send.
         * @param flags Message flags.
         */
        void SendToServer(const std::vector<uint8_t>& data, enet_uint32 flags) override;

        /**
         * @brief Sends data to a client.
         *
         * Pushes a message into the peer's incoming queue with an associated
         * ENet peer pointer.
         *
         * @param peerId Logical peer associated with the message.
         * @param data Payload to send.
         * @param flags Message flags.
         */
        void SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, enet_uint32 flags) override;

        /**
         * @brief Delivers queued messages.
         *
         * Invokes onMessageReceived for each queued message.
         */
        void Poll() override;

        /**
         * @brief Checks whether a peer network is connected.
         *
         * @return true if a peer is set, false otherwise.
         */
        bool Connected() override;

        void Clean() override;
    };
}
