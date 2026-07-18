#include "igneous/networking/SteamNetwork.hpp"

#include <SDL3/SDL_log.h>

#include "igneous/networking/NetworkEvents.hpp"
#include "igneous/networking/NetworkPeerIds.hpp"
#include "igneous/networking/SteamBootstrap.hpp"

namespace Engine
{

#ifdef IGNEOUS_STEAM_ENABLED

    namespace
    {
        ISteamNetworkingSockets* GetSteamSockets()
        {
            if (!SteamBootstrap::IsInitialized())
                return nullptr;
            return SteamNetworkingSockets();
        }
    }

    double SteamNetwork::GetTime()
    {
        using namespace std::chrono;
        return duration<double>(system_clock::now().time_since_epoch()).count();
    }

    int SteamNetwork::ToSteamFlags(TransportType flags)
    {
        return flags == TransportType::Reliable ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable;
    }

    SteamNetwork::~SteamNetwork()
    {
        Clean();
    }

    void SteamNetwork::Connect()
    {
        isServer = true;
        clientWatchdog.enabled = false;

        ISteamNetworkingSockets* sockets = GetSteamSockets();
        if (!sockets)
        {
            SDL_Log("SteamNetwork: Steam not initialized. Call SteamBootstrap::Init() first.");
            loopback.Forward({NetworkEventType::ConnectionFailure});
            return;
        }

        listenSocket = sockets->CreateListenSocketP2P(0, 0, nullptr);
        if (listenSocket == k_HSteamListenSocket_Invalid)
        {
            SDL_Log("SteamNetwork: Failed to create P2P listen socket.");
            loopback.Forward({NetworkEventType::ConnectionFailure});
            return;
        }

        pollGroup = sockets->CreatePollGroup();
        if (pollGroup == k_HSteamNetPollGroup_Invalid)
        {
            SDL_Log("SteamNetwork: Failed to create poll group.");
            loopback.Forward({NetworkEventType::ConnectionFailure});
            return;
        }

        running = true;
        networkThread = std::thread(&SteamNetwork::NetworkLoop, this);
        loopback.Forward({NetworkEventType::ConnectionSuccess});
    }

    void SteamNetwork::Connect(uint64_t hostSteamId)
    {
        if (hostSteamId == 0)
        {
            isServer = false;
            running = true;
            connected = true;
            clientWatchdog.enabled = false;
            loopback.Forward({NetworkEventType::ConnectionSuccess});
            return;
        }

        isServer = false;
        clientWatchdog.enabled = false;

        ISteamNetworkingSockets* sockets = GetSteamSockets();
        if (!sockets)
        {
            SDL_Log("SteamNetwork: Steam not initialized. Call SteamBootstrap::Init() first.");
            loopback.Forward({NetworkEventType::ConnectionFailure});
            return;
        }

        SteamNetworkingIdentity identity{};
        identity.SetSteamID64(hostSteamId);

        serverConnection = sockets->ConnectP2P(identity, 0, 0, nullptr);
        if (serverConnection == k_HSteamNetConnection_Invalid)
        {
            SDL_Log("SteamNetwork: Failed to start connection.");
            loopback.Forward({NetworkEventType::ConnectionFailure});
            return;
        }

        running = true;
        networkThread = std::thread(&SteamNetwork::NetworkLoop, this);
        loopback.Forward({NetworkEventType::ConnectionSuccess});
    }

    void SteamNetwork::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* callback)
    {
        const auto state = callback->m_info.m_eState;
        const uint32_t connId = callback->m_hConn;

        switch (state)
        {
        case k_ESteamNetworkingConnectionState_Connecting:
            if (isServer)
            {
                SteamNetworkingSockets()->AcceptConnection(callback->m_hConn);
                SteamNetworkingSockets()->SetConnectionPollGroup(callback->m_hConn, pollGroup);
            }
            break;
        case k_ESteamNetworkingConnectionState_Connected:
            connected = true;
            connections[connId] = callback->m_hConn;

            if (isServer)
            {
                serverPeers.TrackPeer(connId, GetTime());
                fromNetwork.Push({NetworkEventType::ClientConnected, connId});
            }
            else
            {
                serverConnection = callback->m_hConn;
                const double connectedAt = GetTime();
                clientWatchdog.Reset(connectedAt);
                clientWatchdog.enabled = true;
                fromNetwork.Push({NetworkEventType::ConnectionSuccess});
            }
            break;
        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
            SteamNetworkingSockets()->CloseConnection(callback->m_hConn, 0, nullptr, false);

            if (isServer)
                fromNetwork.Push({NetworkEventType::ClientDisconnected, connId});
            else
                fromNetwork.Push({NetworkEventType::ServerDisconnected});

            connections.erase(connId);
            serverPeers.RemovePeer(connId);
            clientWatchdog.enabled = false;
            break;
        default:
            break;
        }
    }

    void SteamNetwork::NetworkLoop()
    {
        while (running)
        {
            const double now = GetTime();

            if (isServer)
            {
                ServerPing(now);
                DisconnectStaleConnections(now);
            }
            else if (HandleClientTimeout(now))
            {
                return;
            }

            SteamNetworkingSockets()->RunCallbacks();
            ReceiveMessages();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void SteamNetwork::ServerPing(double now)
    {
        if (!serverPeers.TryConsumePingSend(now))
            return;

        for (const auto& [connId, conn]: connections)
        {
            (void) connId;
            SendMessage(conn, ClientConnectionWatchdog::PingPacket, TransportType::Reliable);
        }
    }

    bool SteamNetwork::HandleClientTimeout(double now)
    {
        if (!clientWatchdog.HasTimedOut(now))
            return false;

        SDL_Log("SteamNetwork: server connection timed out.");
        fromNetwork.Push({NetworkEventType::ServerDisconnected});

        if (serverConnection != k_HSteamNetConnection_Invalid)
            SteamNetworkingSockets()->CloseConnection(serverConnection, 0, nullptr, false);
        clientWatchdog.enabled = false;
        running = false;
        return true;
    }

    void SteamNetwork::DisconnectStaleConnections(double now)
    {
        for (uint32_t connId: serverPeers.CollectStalePeerIds(now))
            DisconnectStaleConnection(connId);
    }

    void SteamNetwork::DisconnectStaleConnection(uint32_t connId)
    {
        if (!serverPeers.RemovePeer(connId))
            return;

        fromNetwork.Push({NetworkEventType::ClientDisconnected, connId});

        auto it = connections.find(connId);
        if (it != connections.end())
        {
            SteamNetworkingSockets()->CloseConnection(it->second, 0, nullptr, false);
            connections.erase(it);
        }
    }

    void SteamNetwork::ReceiveMessages()
    {
        SteamNetworkingMessage_t* messages[64];
        int count = 0;

        if (isServer)
            count = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(pollGroup, messages, 64);
        else if (serverConnection != k_HSteamNetConnection_Invalid)
            count = SteamNetworkingSockets()->ReceiveMessagesOnConnection(serverConnection, messages, 64);

        const double now = GetTime();
        for (int i = 0; i < count; ++i)
        {
            SteamNetworkingMessage_t* steamMsg = messages[i];
            const uint8_t* data = static_cast<const uint8_t*>(steamMsg->m_pData);
            const int dataLen = steamMsg->m_cbSize;

            if (dataLen == 1 && data[0] == static_cast<uint8_t>(NetworkEventType::Ping))
            {
                if (isServer)
                    serverPeers.MarkActivity(steamMsg->m_conn, now);
                else
                    clientWatchdog.MarkActivity(now);
                steamMsg->Release();
                continue;
            }

            if (isServer)
                serverPeers.MarkActivity(steamMsg->m_conn, now);
            else
                clientWatchdog.MarkActivity(now);

            NetworkMessage msg;
            msg.type = NetworkEventType::Message;
            msg.peerId = steamMsg->m_conn;
            msg.data.assign(data, data + dataLen);
            steamMsg->Release();
            fromNetwork.Push(std::move(msg));
        }
    }

    void SteamNetwork::SendMessage(HSteamNetConnection conn, const std::vector<uint8_t>& data, TransportType flags)
    {
        SteamNetworkingSockets()->SendMessageToConnection(
                conn,
                data.data(),
                static_cast<uint32_t>(data.size()),
                ToSteamFlags(flags),
                nullptr);
    }

    void SteamNetwork::SendToServer(const std::vector<uint8_t>& data, TransportType flags)
    {
        if (loopback.IsLinked())
        {
            loopback.Forward({NetworkEventType::Message, NetworkPeerIds::Local, data});
            return;
        }

        if (isServer || serverConnection == k_HSteamNetConnection_Invalid)
            return;

        SendMessage(serverConnection, data, flags);
    }

    void SteamNetwork::SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, TransportType flags)
    {
        if (loopback.IsLinked() && peerId == NetworkPeerIds::Local)
        {
            loopback.Forward({NetworkEventType::Message, peerId, data});
            return;
        }

        if (!isServer)
            return;

        auto it = connections.find(peerId);
        if (it == connections.end())
            return;

        SendMessage(it->second, data, flags);
    }

    void SteamNetwork::Poll()
    {
        while (auto msg = fromNetwork.Pop())
        {
            if (onMessageReceived)
                onMessageReceived(*msg);
        }
    }

    bool SteamNetwork::Connected()
    {
        return running && connected;
    }

    void SteamNetwork::Clean()
    {
        running = false;
        clientWatchdog.enabled = false;

        if (networkThread.joinable())
            networkThread.join();

        if (isServer)
        {
            if (pollGroup != k_HSteamNetPollGroup_Invalid)
            {
                SteamNetworkingSockets()->DestroyPollGroup(pollGroup);
                pollGroup = k_HSteamNetPollGroup_Invalid;
            }
            if (listenSocket != k_HSteamListenSocket_Invalid)
            {
                SteamNetworkingSockets()->CloseListenSocket(listenSocket);
                listenSocket = k_HSteamListenSocket_Invalid;
            }
        }
        else if (serverConnection != k_HSteamNetConnection_Invalid)
        {
            SteamNetworkingSockets()->CloseConnection(serverConnection, 0, nullptr, false);
            serverConnection = k_HSteamNetConnection_Invalid;
        }

        connections.clear();
        connected = false;
        loopback.Clear();
    }

#else

    SteamNetwork::~SteamNetwork() = default;

    void SteamNetwork::Connect()
    {
        SDL_Log("SteamNetwork: Steamworks support was not compiled into this build of Igneous. "
                "Enable it with -DIGNEOUS_STEAM=ON and provide the Steamworks SDK.");
    }

    void SteamNetwork::Connect(uint64_t /*hostSteamId*/)
    {
        SDL_Log("SteamNetwork: Steamworks support was not compiled into this build of Igneous. "
                "Enable it with -DIGNEOUS_STEAM=ON and provide the Steamworks SDK.");
    }

    void SteamNetwork::SendToServer(const std::vector<uint8_t>& /*data*/, TransportType /*flags*/)
    {
    }

    void SteamNetwork::SendToClient(uint32_t /*peerId*/, const std::vector<uint8_t>& /*data*/, TransportType /*flags*/)
    {
    }

    void SteamNetwork::Poll()
    {
    }

    bool SteamNetwork::Connected()
    {
        return false;
    }

    void SteamNetwork::Clean()
    {
    }

#endif

}
