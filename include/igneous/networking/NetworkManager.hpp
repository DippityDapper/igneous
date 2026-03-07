#pragma once
#include <memory>

#include "igneous/networking/NetworkInterface.hpp"

namespace Engine
{
    /**
     * @class NetworkManager
     * @brief Factory and helper utilities for networking systems.
     *
     * NetworkManager is responsible for creating concrete NetworkInterface
     * implementations and providing convenience helpers for binding message
     * handlers.
     *
     * It supports both local (in-memory) networking and remote (ENet-based)
     * networking through a unified interface.
     */
    class NetworkManager
    {
      public:
        /**
         * @brief Creates a paired local server and client network.
         *
         * The returned server and client are connected to each other directly
         * in memory, allowing bidirectional message passing without real
         * network sockets.
         *
         * @param outServer Output unique pointer receiving the server interface.
         * @param outClient Output unique pointer receiving the client interface.
         */
        static void CreateLocalNetwork(
                std::unique_ptr<NetworkInterface>& outServer,
                std::unique_ptr<NetworkInterface>& outClient);

        /**
         * @brief Creates a remote server network.
         *
         * Constructs an ENet-based server listening on the given port.
         *
         * @param port Port to bind to.
         * @param maxClients Maximum number of connected clients.
         * @param localOnly Whether to bind only to localhost.
         * @return A unique pointer to the created NetworkInterface.
         */
        static std::unique_ptr<NetworkInterface> CreateServer(
                int port,
                int maxClients,
                bool localOnly = false);

        /**
         * @brief Creates a remote client network.
         *
         * Constructs an ENet-based client and attempts to connect to the
         * specified server.
         *
         * @param port Server port.
         * @param ip Server IP address.
         * @return A unique pointer to the created NetworkInterface.
         */
        static std::unique_ptr<NetworkInterface> CreateClient(
                int port,
                const std::string& ip);

        /**
         * @brief Binds a member function as a network message handler.
         *
         * Assigns the NetworkInterface's onMessageReceived callback to invoke
         * the specified member function on the given instance.
         *
         * @tparam T Class type.
         * @param networkInterface Network interface to bind to.
         * @param instance Object instance receiving messages.
         * @param callback Member function to invoke for each NetworkMessage.
         *
         * @note This overwrites any previously bound message handler.
         * @note The caller is responsible for ensuring the instance remains valid.
         */
        template<typename T>
        static void BindMessageHandler(NetworkInterface* networkInterface, T* instance, void (T::*callback)(const NetworkMessage&));
    };

    /**
     * @brief Template implementation for binding a message handler.
     */
    template<typename T>
    void NetworkManager::BindMessageHandler(NetworkInterface* networkInterface, T* instance, void (T::*callback)(const NetworkMessage&))
    {
        networkInterface->onMessageReceived = [instance, callback](const NetworkMessage& message)
        {
            (instance->*callback)(message);
        };
    }
}
