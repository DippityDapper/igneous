#pragma once

#include <string>
#include <thread>
#include <vector>

#include "enet/enet.h"

#include "dapper2d/ThreadSafeQueue.hpp"

namespace Engine
{
    /// Represents a single network message exchanged between peers.
    struct NetworkMessage
    {
        /// The ENet peer that sent or will receive this message.
        ENetPeer* peer;

        /// The raw message data.
        std::vector<uint8_t> data;

        /// The ENet channel ID associated with the message.
        uint8_t channel = 0;
    };

    /// A utility class for creating and connecting to a host.
    /// Handles both client and server functionality.
    /// It maintains separate message queues for server and client, and runs networking
    /// operations on a dedicated background thread.
    class Networking
    {
    private:
        static std::thread networkThread;
        static std::atomic<bool> running;
        static std::mutex eventMutex;

        /// The messages retrieved from a client by the server.
        static ThreadSafeQueue<NetworkMessage> serverIncomingMessages;

        /// The messages retrieved from the server by the client.
        static ThreadSafeQueue<NetworkMessage> clientIncomingMessages;

        /// The messages waiting to be sent to a peer.
        static ThreadSafeQueue<NetworkMessage> outgoingMessages;

    public:
        /// The ENetHost object for hosting a server.
        static ENetHost* host;

        /// The ENetHost object containing information about the client for the client.
        static ENetHost* client;

        /// The host object for the client. Used to talk with the server from the client.
        static ENetPeer* server;

    public:
        /// Initializes ENet.
        /// @note This is called as part of the Engine initialization.
        static bool Init();

        /// Handles ENet events.
        /// @note This is called in its own thread.
        static void NetworkLoop();

        /// De-initializes ENet and destroys the server.
        /// @note This is called as part of the Engine cleanup.
        static void Clean();

        static void StartNetworkingThread();
        static void StopNetworkingThread();

        /// Creates a server for clients to connect to.
        /// @param port The port to host the server on.
        /// @param peerCount The max number of peers that can connect to the server.
        /// @param localHostOnly whether to host over LAN or WAN.
        /// @param channelLimit The maximum number of channels allowed.
        /// @param incomingBandwidth Downstream bandwidth of the host in bytes/second.
        /// @param outgoingBandwidth Upstream bandwidth of the host in bytes/second.
        /// @return True if the server was successfully created. False otherwise.
        static bool CreateServer(int port, size_t peerCount, bool localHostOnly, size_t channelLimit=0, enet_uint32 incomingBandwidth=0, enet_uint32 outgoingBandwidth=0);

        /// Connect to a server.
        /// @param ip The ip of the server.
        /// @param port The port of the server.
        /// @param data User data supplied to the receiving host.
        /// @return True if the connection succeeded. False otherwise.
        static bool ConnectToServer(const char* ip, size_t port, enet_uint32 data=0);

        /// Send data to a peer.
        /// @param peer The peer to send the data. Send to Network::server to send to the server.
        /// @param data The data to send.
        /// @param channel The ENet channel id.
        static void SendToPeer(ENetPeer* peer, const std::vector<uint8_t>& data, uint8_t channel = 0);

        /// Pops a message from the incoming server messages.
        /// @return An incoming for the server.
        /// @note The message is popped from the queue.
        static std::optional<NetworkMessage> PollServerMessage();

        /// Pops a message from the incoming client messages.
        /// @return An incoming for the client.
        /// @note The message is popped from the queue.
        static std::optional<NetworkMessage> PollClientMessage();
    };
}
