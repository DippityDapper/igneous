#include "dapper2d/Networking.hpp"

#include "SDL3/SDL_log.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_timer.h"

namespace Engine
{
    ENetHost* Networking::host = nullptr;
    ENetHost* Networking::client = nullptr;
    ENetPeer* Networking::server = nullptr;
    std::vector<ENetPeer*> Networking::peers{};

    std::thread Networking::networkThread;
    std::atomic<bool> Networking::running;
    std::mutex Networking::eventMutex;

    ThreadSafeQueue<NetworkMessage> Networking::serverIncomingMessages;
    ThreadSafeQueue<NetworkMessage> Networking::clientIncomingMessages;
    ThreadSafeQueue<NetworkMessage> Networking::outgoingMessages;

    bool Networking::Init()
    {
        if (enet_initialize() < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Enet init failed: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    void Networking::StartNetworkingThread()
    {
        if (running)
            return;

        running = true;
        networkThread = std::thread(NetworkLoop);
    }

    void Networking::StopNetworkingThread()
    {
        if (!running)
            return;

        running = false;
        if (networkThread.joinable())
            networkThread.join();
    }

    void Networking::NetworkLoop()
    {
        ENetEvent event;

        while (running)
        {
            {
                std::lock_guard<std::mutex> lock(eventMutex);

                if (host)
                {
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

                if (client)
                {
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
                                break;
                            }
                        default:
                            break;
                        }
                    }
                }
            }

            while (const auto& msg = outgoingMessages.Pop())
            {
                ENetPacket* packet = enet_packet_create(msg->data.data(), msg->data.size(), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(msg->peer, msg->channel, packet);
            }

            SDL_Delay(1);
        }
    }

    void Networking::SendToPeer(ENetPeer* peer, const std::vector<uint8_t>& data, uint8_t channel)
    {
        if (!peer)
            return;
        outgoingMessages.Push({ peer, data, channel });
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

    bool Networking::ConnectToServer(const char* ip, size_t port, enet_uint32 data)
    {
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

    void Networking::Clean()
    {
        StopNetworkingThread();

        if (host)
            enet_host_destroy(host);
        if (client)
            enet_host_destroy(client);
        enet_deinitialize();
    }
}
