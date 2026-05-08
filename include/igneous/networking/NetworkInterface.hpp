#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "igneous/networking/NetworkEvents.hpp"

#include <queue>

namespace Engine
{
    /**
     * @class NetworkInterface
     * @brief Abstract interface for networking backends.
     *
     * NetworkInterface defines a common API for sending, receiving, and polling
     * network messages. Concrete implementations may represent local, simulated,
     * or remote (ENet-based) networking systems.
     *
     * Messages received from the network are delivered through the
     * onMessageReceived callback when Poll() is called.
     */
    class NetworkInterface
    {
      protected:
        NetworkInterface* loopbackPeer = nullptr;

      public:
        /**
         * @brief Callback invoked when a network message is received.
         *
         * This callback is triggered during Poll() for each pending
         * NetworkMessage.
         *
         * @note The callback is optional and may be unset.
         */
        std::function<void(const NetworkMessage&)> onMessageReceived;

        std::queue<NetworkMessage> loopbackMessages{};

      public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~NetworkInterface() = default;

        /**
         * @brief Sends data to the server.
         *
         * For client implementations, this transmits data to the connected server.
         * For server implementations, this may be a no-op.
         *
         * @param data Payload to send.
         * @param flags ENet packet flags.
         */
        virtual void SendToServer(const std::vector<uint8_t>& data, uint32_t flags) = 0;

        /**
         * @brief Sends data to a specific client.
         *
         * Typically only valid for server implementations.
         *
         * @param peerId Target peer id.
         * @param data Payload to send.
         * @param flags ENet packet flags.
         */
        virtual void SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, uint32_t flags) = 0;

        /**
         * @brief Processes pending network events.
         *
         * Implementations should invoke onMessageReceived for each queued
         * NetworkMessage.
         */
        virtual void Poll() = 0;

        /**
         * @brief Checks whether the network connection is active.
         *
         * @return true if connected or running, false otherwise.
         */
        virtual bool Connected() = 0;

        virtual void Clean() = 0;

        void SetLoopbackPeer(NetworkInterface* _loopbackPeer)
        {
            loopbackPeer = _loopbackPeer;
        }
    };
}
