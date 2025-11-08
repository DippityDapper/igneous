#include "dapper2d/Networking.hpp"

#include <filesystem>

#include "SDL3/SDL_log.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_process.h"
#include "SDL3/SDL_timer.h"

namespace Engine
{
    ENetHost* Networking::host = nullptr;
    ENetHost* Networking::client = nullptr;
    ENetPeer* Networking::server = nullptr;
    SDL_Process* Networking::serverProcess;

    std::thread Networking::clientThread;
    std::atomic<bool> Networking::clientThreadRunning;
    std::mutex Networking::clientEventMutex;

    std::thread Networking::serverThread;
    std::atomic<bool> Networking::serverThreadRunning;
    std::mutex Networking::serverEventMutex;

    ThreadSafeQueue<NetworkMessage> Networking::serverIncomingMessages;
    ThreadSafeQueue<NetworkMessage> Networking::clientIncomingMessages;
    ThreadSafeQueue<NetworkMessage> Networking::serverOutgoingMessages;
    ThreadSafeQueue<NetworkMessage> Networking::clientOutgoingMessages;

    bool Networking::Init()
    {
        if (enet_initialize() < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Enet init failed: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    bool Networking::StartClientThread()
    {
        if (clientThreadRunning)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Client thread already running");
            return false;
        }

        clientThreadRunning = true;
        clientThread = std::thread(HandleClientEvents);
        return true;
    }

    bool Networking::StartServerThread()
    {
        if (serverThreadRunning)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Server thread already running");
            return false;
        }

        serverThreadRunning = true;
        serverThread = std::thread(HandleServerEvents);
        return true;
    }

    void Networking::StopClientThread()
    {
        if (!clientThreadRunning)
            return;

        clientThreadRunning = false;
        if (clientThread.joinable())
            clientThread.join();
    }

    void Networking::StopServerThread()
    {
        if (!serverThreadRunning)
            return;

        serverThreadRunning = false;
        if (serverThread.joinable())
            serverThread.join();
    }

    void Networking::HandleServerEvents()
    {
        while (!host && serverThreadRunning)
            SDL_Delay(10);

        ENetEvent event;

        while (serverThreadRunning)
        {
            {
                std::lock_guard<std::mutex> lock(serverEventMutex);

                while (enet_host_service(host, &event, 1) > 0)
                {
                    switch (event.type)
                    {
                    case ENET_EVENT_TYPE_CONNECT:
                        {
                            break;
                        }
                    case ENET_EVENT_TYPE_RECEIVE:
                        {
                            std::vector<uint8_t> buffer(event.packet->data, event.packet->data + event.packet->dataLength);
                            serverIncomingMessages.Push({ event.peer, buffer, event.channelID });

                            enet_packet_destroy(event.packet);
                            break;
                        }
                    case ENET_EVENT_TYPE_DISCONNECT:
                        {
                            break;
                        }
                    default:
                        break;
                    }
                }
            }

            while (const auto& msg = serverOutgoingMessages.Pop())
            {
                ENetPacket* packet = enet_packet_create(msg->data.data(), msg->data.size(), ENET_PACKET_FLAG_RELIABLE);
                if (msg->broadcast)
                    enet_host_broadcast(host, msg->channel, packet);
                else
                    enet_peer_send(msg->peer, msg->channel, packet);
            }

            SDL_Delay(1);
        }
    }

    void Networking::HandleClientEvents()
    {
        while (!client && clientThreadRunning)
            SDL_Delay(10);

        ENetEvent event;

        while (clientThreadRunning)
        {
            {
                std::lock_guard<std::mutex> lock(clientEventMutex);

                while (enet_host_service(client, &event, 1) > 0)
                {
                    switch (event.type)
                    {
                    case ENET_EVENT_TYPE_CONNECT:
                        {
                            break;
                        }
                    case ENET_EVENT_TYPE_RECEIVE:
                        {
                            std::vector<uint8_t> buffer(event.packet->data, event.packet->data + event.packet->dataLength);
                            clientIncomingMessages.Push({ event.peer, buffer, event.channelID });

                            enet_packet_destroy(event.packet);
                            break;
                        }
                    case ENET_EVENT_TYPE_DISCONNECT:
                        {
                            SDL_Log("Disconnected from server.");

                            if (client)
                            {
                                enet_host_destroy(client);
                                client = nullptr;
                            }

                            if (host)
                            {
                                enet_host_destroy(host);
                                host = nullptr;
                            }

                            if (server)
                            {
                                server = nullptr;
                            }

                            return;
                        }
                    default:
                        break;
                    }
                }
            }

            while (const auto& msg = clientOutgoingMessages.Pop())
            {
                ENetPacket* packet = enet_packet_create(msg->data.data(), msg->data.size(), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(msg->peer, msg->channel, packet);
            }

            SDL_Delay(1);
        }
    }


    void Networking::ClientSendToPeer(ENetPeer* peer, const std::vector<uint8_t>& data, uint8_t channel)
    {
        if (!peer)
            return;
        clientOutgoingMessages.Push({ peer, data, channel });
    }

    void Networking::ServerSendToPeer(ENetPeer* peer, const std::vector<uint8_t>& data, uint8_t channel)
    {
        if (!peer)
            return;
        serverOutgoingMessages.Push({ peer, data, channel });
    }

    void Networking::ServerBroadcast(const std::vector<uint8_t>& data, uint8_t channel)
    {
        if (!host)
            return;
        serverOutgoingMessages.Push({nullptr, data, channel, true});
    }

    std::optional<NetworkMessage> Networking::PollServerMessage()
    {
        return serverIncomingMessages.Pop();
    }

    std::optional<NetworkMessage> Networking::PollClientMessage()
    {
        return clientIncomingMessages.Pop();
    }

    bool Networking::CreateServer(const int port, const size_t peerCount, const bool localHostOnly, const size_t channelLimit, enet_uint32 incomingBandwidth, enet_uint32 outgoingBandwidth)
    {
        ENetAddress address;

        address.port = port;
        if (localHostOnly)
            enet_address_set_host_ip(&address, "127.0.0.1");
        else
            address.host = ENET_HOST_ANY;

        host = enet_host_create(&address, peerCount, channelLimit, incomingBandwidth, outgoingBandwidth);
        if (!host)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create server: %s.", SDL_GetError());
            return false;
        }

        return true;
    }

    bool Networking::CreateServerProcess(const char* port, const char* worldName, const char* peerCount, bool localOnly)
    {
        if (serverProcess)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Server process already running.");
            return false;
        }

        std::string basePath = SDL_GetBasePath();

        std::string exePath{basePath};

#if defined(_WIN32)
        exePath += "../server/server.exe";
#else
        exePath += "../server/server";
#endif

        if (!std::filesystem::exists(exePath))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Server executable not found: %s", exePath.c_str());
            return false;
        }

        const char* localOnlyChar = localOnly ? "--local-only" : "";
        const char* args[] = { exePath.c_str(), port, worldName, "--peer-count", peerCount, localOnlyChar, nullptr };

        serverProcess = SDL_CreateProcess(args, false);
        if (!serverProcess)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create server process: %s.", SDL_GetError());
            return false;
        }
        return true;
    }

    void Networking::StopServerProcess()
    {
        if (serverProcess)
        {
            SDL_KillProcess(serverProcess, false);
            SDL_WaitProcess(serverProcess, false, nullptr);
            serverProcess = nullptr;
        }
    }

    bool Networking::ConnectToServer(const char* ip, size_t port, enet_uint32 data)
    {
        if (client || server)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Client already running.");
            return false;
        }

        client = enet_host_create(nullptr, 1, 2, 0, 0);

        if (!client)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create client: %s.", SDL_GetError());
            return false;
        }

        ENetAddress address;

        enet_address_set_host_ip(&address, ip);
        address.port = port;

        server = enet_host_connect(client, &address, 2, data);
        if (!server)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create peer: %s.", SDL_GetError());
            return false;
        }

        ENetEvent event;
        if (!(enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to connect to server.");
            enet_peer_reset(server);
            return false;
        }

        return true;
    }

    bool Networking::DisconnectFromServer()
    {
        if (!server && !client)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to disconnect from server.");
            return false;
        }

        enet_host_destroy(client);

        client = nullptr;
        server = nullptr;

        return true;
    }

    void Networking::Clean()
    {
        StopClientThread();
        StopServerThread();

        StopServerProcess();

        if (host)
            enet_host_destroy(host);
        if (client)
            enet_host_destroy(client);

        enet_deinitialize();
    }
}
