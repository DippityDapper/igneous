// Doc: docs/classes/NetworkSessionFactory.md
#pragma once

#include <memory>
#include <type_traits>

#include "igneous/networking/ENetNetwork.hpp"
#include "igneous/networking/ILoopbackNetwork.hpp"
#include "igneous/networking/LocalNetwork.hpp"
#include "igneous/networking/SteamNetwork.hpp"

namespace Engine
{

    class NetworkSessionFactory
    {
      public:
        static constexpr int EnetPort = 33333;
        static constexpr int EnetPeerCount = 20;

        template<typename A, typename B>
        static void LinkLoopback(A& a, B& b)
        {
            static_assert(std::is_base_of_v<ILoopbackNetwork, A>, "A must implement ILoopbackNetwork");
            static_assert(std::is_base_of_v<NetworkInterface, A>, "A must implement NetworkInterface");
            static_assert(std::is_base_of_v<ILoopbackNetwork, B>, "B must implement ILoopbackNetwork");
            static_assert(std::is_base_of_v<NetworkInterface, B>, "B must implement NetworkInterface");

            a.GetLoopback().SetPeer(&b);
            b.GetLoopback().SetPeer(&a);
        }

        struct LocalClientServer
        {
            std::unique_ptr<LocalNetwork> server;
            std::unique_ptr<LocalNetwork> client;
        };

        static LocalClientServer CreateLocalClientServer()
        {
            LocalClientServer session;
            session.server = std::make_unique<LocalNetwork>();
            session.client = std::make_unique<LocalNetwork>();
            LinkLoopback(*session.server, *session.client);
            session.server->Connect(true);
            session.client->Connect(false);
            return session;
        }

        struct EnetClientServer
        {
            std::unique_ptr<ENetNetwork> server;
            std::unique_ptr<ENetNetwork> client;
        };

        static EnetClientServer CreateEnetClientServer(bool localOnly)
        {
            EnetClientServer session;
            session.server = std::make_unique<ENetNetwork>();
            session.client = std::make_unique<ENetNetwork>();
            LinkLoopback(*session.server, *session.client);
            session.server->Connect(EnetPort, EnetPeerCount, localOnly);
            session.client->Connect();
            return session;
        }

        struct SteamClientServer
        {
            std::unique_ptr<SteamNetwork> server;
            std::unique_ptr<SteamNetwork> client;
        };

        static SteamClientServer CreateSteamClientServer()
        {
            SteamClientServer session;
            session.server = std::make_unique<SteamNetwork>();
            session.client = std::make_unique<SteamNetwork>();
            LinkLoopback(*session.server, *session.client);
            session.server->Connect();
            session.client->Connect(0);
            return session;
        }
    };
}
