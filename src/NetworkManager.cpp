#include "dapper2d/NetworkManager.hpp"

namespace Engine
{
    std::map<PacketType, std::vector<std::function<void(ENetEvent&)>>> NetworkManager::packetHandlers;
    std::vector<std::function<void(int, NetworkManager*)>> NetworkManager::disconnectionEvent;

    SDL_AppResult NetworkManager::Init()
    {
        if (enet_initialize() != 0)
        {
            SDL_Log("An error occurred while initializing ENet: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        InitPacketHandlers();

        return SDL_APP_CONTINUE;
    }

    void NetworkManager::InitPacketHandlers()
    {
        NetworkManager::AddHandler(PACKET_CLIENT_DATA, [this](ENetEvent& event)
        {
            HandleClientDataPacket(event);
        });
        NetworkManager::AddHandler(PACKET_DISCONNECT, [this](ENetEvent& event)
        {
            HandleDisconnectPacket(event);
        });
    }

    void NetworkManager::AddHandler(PacketType pType, const std::function<void(ENetEvent &)>& func)
    {
        packetHandlers[pType].emplace_back(func);
    }

    SDL_AppResult NetworkManager::CreateClient()
    {
        client = enet_host_create(nullptr, 1, 2, 0, 0);

        if (!client)
        {
            SDL_Log("An error occurred while trying to create an ENet client ip: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    void NetworkManager::Disconnect() const
    {
        DisconnectPacket disconnectPacket;
        disconnectPacket.type = PACKET_DISCONNECT;
        disconnectPacket.clientId = clientId;
        SDL_Log("Peer disconnected: %d", clientId);

        ENetPacket* packet = enet_packet_create(&disconnectPacket, sizeof(DisconnectPacket), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(server, 0, packet);
        enet_host_flush(client);
    }

    SDL_AppResult NetworkManager::ConnectToServer(int port, std::string& ip)
    {
        ENetAddress address;

        enet_address_set_host(&address, ip.c_str());
        SDL_Log("Address: %u", address.host);
        address.port = port;

        server = enet_host_connect(client, &address, 2, 0);
        if (!server)
        {
            SDL_Log("No available peers for initiating connection: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        SDL_Log("Attempting to connect to server...");

        ENetEvent event;
        bool connected = false;
        uint32_t connectionTimeout = SDL_GetTicks() + 5000;

        while (!connected && SDL_GetTicks() < connectionTimeout)
        {
            while (enet_host_service(client, &event, 100) > 0)
            {
                switch (event.type)
                {
                    case ENET_EVENT_TYPE_CONNECT:
                        SDL_Log("Connection to server succeeded");
                        connected = true;
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        SDL_Log("Connection to server failed");
                        return SDL_APP_FAILURE;
                    default:
                        break;
                }
            }
        }

        if (!connected)
        {
            SDL_Log("Connection to server timed out");
            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    bool NetworkManager::HandleNetworkEvents()
    {
        ENetEvent enetEvent;
        while (enet_host_service(client, &enetEvent, 0) > 0)
        {
            switch (enetEvent.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    return HandlePackets(enetEvent);
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Disconnected from server.");
                    return false;
                }
                default:
                    break;
            }
        }
        return true;
    }

    bool NetworkManager::HandlePackets(ENetEvent enetEvent)
    {
        auto* p_packetType = reinterpret_cast<PacketType*>(enetEvent.packet->data);
        PacketType packetType = *p_packetType;

        if (packetHandlers.contains(packetType))
        {
            std::vector<std::function<void(ENetEvent&)>> callers = packetHandlers[packetType];
            for (auto& caller : callers)
            {
                caller(enetEvent);
            }
        }
        else
        {
            SDL_Log("Unhandled packet type: %d", packetType);
        }

        enet_packet_destroy(enetEvent.packet);
        return true;
    }

    void NetworkManager::HandleClientDataPacket(ENetEvent& event)
    {
        ClientDataPacket* clientDataPacket = (ClientDataPacket*)event.packet->data;
        SDL_Log("Client id: %d", clientDataPacket->clientId);

        clientId = clientDataPacket->clientId;
    }

    void NetworkManager::HandleDisconnectPacket(ENetEvent& event)
    {
        DisconnectPacket* disconnectPacket = (DisconnectPacket*)event.packet->data;
        SDL_Log("Peer disconnected: %d", clientId);

        for (auto& caller : disconnectionEvent)
        {
            caller(disconnectPacket->clientId, this);
        }
    }

    void NetworkManager::Clean()
    {
        enet_host_destroy(client);
        enet_deinitialize();
    }
}
