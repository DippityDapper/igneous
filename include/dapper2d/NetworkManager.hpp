#pragma once

#include <map>
#include <functional>
#include <string>

#include "SDL3/SDL.h"
#include "enet/enet.h"

#include "dapper2d/Packets.hpp"

namespace Engine
{
    class NetworkManager
    {
    public:
        int clientId = -1;
        char username[32] = "";

        bool triedConnect = false;

        ENetHost* client = nullptr;
        ENetPeer* server = nullptr;

        static std::map<PacketType, std::vector<std::function<void(ENetEvent&)>>> packetHandlers;
        static std::vector<std::function<void(int, NetworkManager*)>> disconnectionEvent;

    public:
        SDL_AppResult Init();
        void Clean();

        void InitPacketHandlers();
        static void AddHandler(PacketType pType, const std::function<void(ENetEvent&)>& func);

        SDL_AppResult CreateClient();
        SDL_AppResult ConnectToServer(int port, std::string& ip);

        void Disconnect() const;

        bool HandleNetworkEvents();
        bool HandlePackets(ENetEvent enetEvent);

        void HandleClientDataPacket(ENetEvent& event);
        void HandleDisconnectPacket(ENetEvent& event);
    };
}
