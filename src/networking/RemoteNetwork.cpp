#include "igneous/networking/RemoteNetwork.hpp"

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"

namespace Engine
{
    RemoteNetwork::RemoteNetwork(int port, int peerCount, bool localOnly)
    {
        isServer = true;

        ENetAddress address;

        address.port = port;
        if (localOnly)
            enet_address_set_host_ip(&address, "127.0.0.1");
        else
            address.host = ENET_HOST_ANY;

        host = enet_host_create(&address, peerCount, 2, 0, 0);
        if (!host)
        {
            SDL_Log("Failed to create server: %s.", SDL_GetError());
            return;
        }

        running = true;
        networkThread = std::thread(&RemoteNetwork::NetworkLoop, this);
    }

    RemoteNetwork::RemoteNetwork(int port, const std::string& ip)
    {
        isServer = false;

        host = enet_host_create(nullptr, 1, 2, 0, 0);

        if (!host)
        {
            SDL_Log("Failed to create client: %s.", SDL_GetError());
            enet_host_destroy(host);
            host = nullptr;
            return;
        }

        ENetAddress address;

        enet_address_set_host_ip(&address, ip.c_str());
        address.port = port;

        serverPeer = enet_host_connect(host, &address, 2, 0);
        if (!serverPeer)
        {
            SDL_Log("Failed to create peer: %s.", SDL_GetError());
            enet_host_destroy(host);
            host = nullptr;
            return;
        }

        ENetEvent event;
        if (!(enet_host_service(host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT))
        {
            SDL_Log("Failed to connect to server.");
            enet_host_destroy(host);
            host = nullptr;
            serverPeer = nullptr;
            return;
        }

        running = true;
        networkThread = std::thread(&RemoteNetwork::NetworkLoop, this);
    }

    RemoteNetwork::~RemoteNetwork()
    {
        running = false;
        if (networkThread.joinable())
            networkThread.join();

        if (host)
            enet_host_destroy(host);
    }

    void RemoteNetwork::NetworkLoop()
    {
        while (running)
        {
            ENetEvent event;
            while (enet_host_service(host, &event, 1) > 0)
            {
                if (isServer)
                    HandleServerEvent(event);
                else
                    HandleClientEvent(event);
            }

            enet_host_flush(host);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void RemoteNetwork::HandleServerEvent(const ENetEvent& event)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                NetworkMessage msg;
                msg.type = NetworkEventType::ClientConnected;
                msg.peer = event.peer;
                fromNetwork.Push(msg);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                NetworkMessage msg;
                msg.type = NetworkEventType::ClientDisconnected;
                msg.peer = event.peer;
                fromNetwork.Push(msg);
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                NetworkMessage msg;
                msg.type = NetworkEventType::Message;
                msg.peer = event.peer;
                msg.data.assign(event.packet->data, event.packet->data + event.packet->dataLength);
                fromNetwork.Push(msg);
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }

    void RemoteNetwork::HandleClientEvent(const ENetEvent& event)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                NetworkMessage msg;
                msg.type = NetworkEventType::ClientConnected;
                msg.peer = event.peer;
                fromNetwork.Push(msg);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                NetworkMessage msg;
                msg.type = NetworkEventType::ClientDisconnected;
                msg.peer = event.peer;
                fromNetwork.Push(msg);
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                NetworkMessage msg;
                msg.type = NetworkEventType::Message;
                msg.peer = event.peer;
                msg.data.assign(event.packet->data, event.packet->data + event.packet->dataLength);
                fromNetwork.Push(msg);
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }

    void RemoteNetwork::SendPacket(ENetPeer* peer, std::vector<uint8_t> data, enet_uint32 packetType)
    {
        ENetPacket* enetPacket = enet_packet_create(data.data(), data.size(), packetType);
        enet_peer_send(peer, 0, enetPacket);
    }

    void RemoteNetwork::Poll()
    {
        while (const auto& msg = fromNetwork.Pop())
        {
            if (onMessageReceived)
                onMessageReceived(*msg);
        }
    }

    bool RemoteNetwork::Connected()
    {
        return running;
    }

    void RemoteNetwork::SendToClient(ENetPeer* peer, const std::vector<uint8_t>& data, enet_uint32 flags)
    {
        if (!isServer)
            return;

        ENetPacket* packet = enet_packet_create(data.data(), data.size(), flags);
        enet_peer_send(peer, 0, packet);
    }

    void RemoteNetwork::SendToServer(const std::vector<uint8_t>& data, enet_uint32 flags)
    {
        if (isServer || !serverPeer)
            return;

        ENetPacket* packet = enet_packet_create(data.data(), data.size(), flags);
        enet_peer_send(serverPeer, 0, packet);
    }
}
