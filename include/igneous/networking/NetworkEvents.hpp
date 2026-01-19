#pragma once

#include <vector>

#include "enet/enet.h"
#include "enet/types.h"

namespace Engine
{
    /**
     * @enum NetworkEventType
     * @brief Represents the type of network event for a message.
     *
     * Used in NetworkMessage to indicate whether the message is a standard
     * message or a connection/disconnection event.
     */
    enum class NetworkEventType : uint8_t
    {
        /**
         * @brief A standard network message containing data.
         */
        Message = 0,

        /**
         * @brief Indicates that a client has connected.
         */
        ClientConnected = 1,

        /**
         * @brief Indicates that a client has disconnected.
         */
        ClientDisconnected = 2,

        /**
         * @brief Indicates that the server has disconnected.
         */
        ServerDisconnected = 3
    };

    /**
     * @struct NetworkMessage
     * @brief Represents a single network event or message.
     *
     * NetworkMessage encapsulates both data messages and connection-related
     * events. It is used by NetworkInterface implementations to queue and
     * process network traffic.
     *
     * @see Engine::NetworkInterface
     * @see Engine::LocalNetwork
     * @see Engine::RemoteNetwork
     */
    struct NetworkMessage
    {
        /**
         * @brief Type of event (message, client/server connection/disconnection).
         */
        NetworkEventType type = NetworkEventType::Message;

        /**
         * @brief ENet peer associated with the message, or nullptr for
         * server-originated messages.
         */
        ENetPeer* peer = nullptr;

        /**
         * @brief Payload data for Message events.
         */
        std::vector<uint8_t> data{};

        /**
         * @brief Flags associated with ENet packet transmission (ENet packet types).
         */
        enet_uint32 flags = 0;
    };
}
