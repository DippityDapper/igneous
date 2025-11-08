#pragma once

#include <string>
#include <thread>
#include <vector>

#include "enet/enet.h"

#include "dapper2d/ThreadSafeQueue.hpp"

struct SDL_Process;

namespace Engine
{
    /// Represents a single network message exchanged between peers.
    struct NetworkMessage
    {
        /// The ENet peer that will receive this message.
        ENetPeer* peer;

        /// The raw message data.
        std::vector<uint8_t> data;

        /// The ENet channel ID associated with the message.
        uint8_t channel = 0;

        /// Should this message be sent to all peers.
        bool broadcast = false;
    };

    /// A utility class for creating and connecting to a host.
    /// Handles both client and server functionality.
    /// It maintains separate message queues for server and client, and runs networking
    /// operations on a dedicated background thread.
    class Networking
    {
    private:
        static std::thread clientThread;
        static std::atomic<bool> clientThreadRunning;
        static std::mutex clientEventMutex;

        static std::thread serverThread;
        static std::atomic<bool> serverThreadRunning;
        static std::mutex serverEventMutex;

        /// The messages retrieved from a client by the server.
        static ThreadSafeQueue<NetworkMessage> serverIncomingMessages;

        /// The messages retrieved from the server by the client.
        static ThreadSafeQueue<NetworkMessage> clientIncomingMessages;

        /// The server messages waiting to be sent to a peer.
        static ThreadSafeQueue<NetworkMessage> serverOutgoingMessages;

        /// The client messages waiting to be sent to a peer.
        static ThreadSafeQueue<NetworkMessage> clientOutgoingMessages;

    public:
        /// The ENetHost object for hosting a server.
        static ENetHost* host;

        /// The ENetHost object containing information about the client for the client.
        static ENetHost* client;

        /// The host object for the client. Used to talk with the server from the client.
        static ENetPeer* server;

        static SDL_Process* serverProcess;

    public:
        /// Initializes ENet.
        /// @note This is called as part of the Engine initialization.
        static bool Init();

        /// Handles the servers ENet events.
        /// @note This is called in its own thread.
        static void HandleServerEvents();

        /// Handles the clients ENet events.
        /// @note Call StartNetworkingThread to run this function in its own thread.
        static void HandleClientEvents();

        /// De-initializes ENet and destroys the server.
        /// @note This is called as part of the Engine cleanup.
        static void Clean();

        /// Start a thread for client networking events.
        static bool StartClientThread();

        /// Start a thread for server networking events.
        static bool StartServerThread();

        /// Stop the networking thread.
        static void StopClientThread();

        /// Stop the networking thread.
        static void StopServerThread();

        /// Creates a server for clients to connect to.
        /// @param port The port to host the server on.
        /// @param peerCount The max number of peers that can connect to the server.
        /// @param localHostOnly whether to host over LAN or WAN.
        /// @param channelLimit The maximum number of channels allowed.
        /// @param incomingBandwidth Downstream bandwidth of the host in bytes/second.
        /// @param outgoingBandwidth Upstream bandwidth of the host in bytes/second.
        /// @return True if the server was successfully created. False otherwise.
        static bool CreateServer(int port, size_t peerCount, bool localHostOnly, size_t channelLimit=0, enet_uint32 incomingBandwidth=0, enet_uint32 outgoingBandwidth=0);

        /// Creates a separate server process.
        /// @param port  The port to host the server on.
        /// @param peerCount The max number of peers that can connect to the server.
        /// @param localOnly whether to host over LAN or WAN.
        /// @return True if the server process was successfully created. False otherwise.
        static bool CreateServerProcess(const char* port, const char* worldName, const char* peerCount, bool localOnly);

        /// Kills the server process.
        static void StopServerProcess();

        /// Connect to a server.
        /// @param ip The ip of the server.
        /// @param port The port of the server.
        /// @param data User data supplied to the receiving host.
        /// @return True if the connection succeeded. False otherwise.
        static bool ConnectToServer(const char* ip, size_t port, enet_uint32 data=0);

        /// Disconnect from the current server.
        static bool DisconnectFromServer();

        /// For the client to send data to a peer.
        /// @param peer The peer to send the data. Send to Network::server to send to the server.
        /// @param data The data to send.
        /// @param channel The ENet channel id.
        static void ClientSendToPeer(ENetPeer* peer, const std::vector<uint8_t>& data, uint8_t channel = 0);

        /// For the server to send data to a peer.
        /// @param peer The peer to send the data. Send to Network::server to send to the server.
        /// @param data The data to send.
        /// @param channel The ENet channel id.
        static void ServerSendToPeer(ENetPeer* peer, const std::vector<uint8_t>& data, uint8_t channel = 0);

        /// For the server to send data to a peer.
        /// @param data The data to send.
        /// @param channel The ENet channel id.
        static void ServerBroadcast(const std::vector<uint8_t>& data, uint8_t channel = 0);

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
