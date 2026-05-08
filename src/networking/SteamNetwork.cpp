#include "igneous/networking/SteamNetwork.hpp"

#include "upnpcommands.h"

#include <SDL3/SDL_log.h>

#include "igneous/networking/NetworkEvents.hpp"

#include <ranges>

namespace Engine
{

    // =============================================================================
    // Steam-enabled implementation
    // =============================================================================

#ifdef IGNEOUS_STEAM_ENABLED

    const std::vector<uint8_t> SteamNetwork::PingPacket = {static_cast<uint8_t>(NetworkEventType::Ping)};

    double SteamNetwork::GetTime() const
    {
        using namespace std::chrono;
        return duration<double>(system_clock::now().time_since_epoch()).count();
    }

    // -------------------------------------------------------------------------
    // Server constructor
    // -------------------------------------------------------------------------

    SteamNetwork::SteamNetwork(int port, bool localOnly)
    {
        _isServer = true;
        _port = port;

        if (!localOnly)
        {
            int error = 0;
            _upnpDevList = upnpDiscover(2000, nullptr, nullptr, UPNP_LOCAL_PORT_ANY, 0, 2, &error);

            if (_upnpDevList)
            {
                char lanAddr[64] = {};
                char wanAddr[64] = {};
                int igdStatus = UPNP_GetValidIGD(_upnpDevList, &_upnpUrls, &_upnpData, lanAddr, sizeof(lanAddr), wanAddr, sizeof(wanAddr));

                if (igdStatus == 1)
                {
                    const std::string portStr = std::to_string(port);
                    int mapResult = UPNP_AddPortMapping(
                            _upnpUrls.controlURL,
                            _upnpData.first.servicetype,
                            portStr.c_str(), // external port
                            portStr.c_str(), // internal port
                            lanAddr,         // internal client (our LAN IP)
                            "Interspace",    // description
                            "UDP",           // protocol — change to TCP if needed
                            nullptr,         // remote host (nullptr = any)
                            "0"              // lease duration (0 = indefinite)
                    );

                    if (mapResult == UPNPCOMMAND_SUCCESS)
                    {
                        _upnpMapped = true;

                        char externalIP[40] = {};
                        UPNP_GetExternalIPAddress(
                                _upnpUrls.controlURL,
                                _upnpData.first.servicetype,
                                externalIP);
                        SDL_Log("UPnP: Port %d mapped successfully. External IP: %s", port, externalIP);
                    }
                    else
                    {
                        SDL_Log("UPnP: Port mapping failed with code %d", mapResult);
                    }
                }
                else
                {
                    SDL_Log("UPnP: No valid connected gateway found (IGD status %d).", igdStatus);
                }
            }
            else
            {
                SDL_Log("UPnP: Discovery failed (error %d).", error);
            }
        }

        SteamNetworkingIPAddr addr{};
        addr.Clear();
        addr.m_port = static_cast<uint16_t>(port);

        if (localOnly)
            addr.ParseString("127.0.0.1");

        _listenSocket = SteamNetworkingSockets()->CreateListenSocketIP(addr, 0, nullptr);
        if (_listenSocket == k_HSteamListenSocket_Invalid)
        {
            SDL_Log("SteamNetwork: Failed to create listen socket.");
            _fromNetwork.Push({NetworkEventType::ConnectionFailure});
            return;
        }

        _pollGroup = SteamNetworkingSockets()->CreatePollGroup();
        if (_pollGroup == k_HSteamNetPollGroup_Invalid)
        {
            SDL_Log("SteamNetwork: Failed to create poll group.");
            SteamNetworkingSockets()->CloseListenSocket(_listenSocket);
            _listenSocket = k_HSteamListenSocket_Invalid;
            _fromNetwork.Push({NetworkEventType::ConnectionFailure});
            return;
        }

        _running = true;
        _fromNetwork.Push({NetworkEventType::ConnectionSuccess});
        _networkThread = std::thread(&SteamNetwork::NetworkLoop, this);
    }

    // -------------------------------------------------------------------------
    // Client constructor
    // -------------------------------------------------------------------------

    SteamNetwork::SteamNetwork(int port, const std::string& ip)
    {
        _isServer = false;

        SteamNetworkingIPAddr addr{};
        addr.ParseString(ip.c_str());
        addr.m_port = static_cast<uint16_t>(port);

        _serverConnection = SteamNetworkingSockets()->ConnectByIPAddress(addr, 0, nullptr);
        if (_serverConnection == k_HSteamNetConnection_Invalid)
        {
            SDL_Log("SteamNetwork: Failed to connect to %s:%d.", ip.c_str(), port);
            _fromNetwork.Push({NetworkEventType::ConnectionFailure});
            return;
        }

        _running = true;
        _networkThread = std::thread(&SteamNetwork::NetworkLoop, this);
    }

    // -------------------------------------------------------------------------
    // Loopback constructor
    // -------------------------------------------------------------------------

    SteamNetwork::SteamNetwork()
    {
        _isServer = false;
        _running = true;
        _fromNetwork.Push({NetworkEventType::ConnectionSuccess});
    }

    SteamNetwork::~SteamNetwork()
    {
        SteamNetwork::Clean();
    }

    // -------------------------------------------------------------------------
    // Connection status callback (runs on the network thread via RunCallbacks)
    // -------------------------------------------------------------------------

    void SteamNetwork::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pCallback)
    {
        auto state = pCallback->m_info.m_eState;
        uint32_t connId = pCallback->m_hConn;

        switch (state)
        {
        case k_ESteamNetworkingConnectionState_Connecting:
        {
            if (_isServer)
            {
                SteamNetworkingSockets()->AcceptConnection(pCallback->m_hConn);
                SteamNetworkingSockets()->SetConnectionPollGroup(pCallback->m_hConn, _pollGroup);
            }
            break;
        }
        case k_ESteamNetworkingConnectionState_Connected:
        {
            _connected = true;
            _connections[connId] = pCallback->m_hConn;

            if (_isServer)
            {
                _fromNetwork.Push({NetworkEventType::ClientConnected, connId});
            }
            else
            {
                _serverConnection = pCallback->m_hConn;
                _lastPingTime = GetTime();
                _fromNetwork.Push({NetworkEventType::ConnectionSuccess});
            }
            break;
        }
        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        {
            SteamNetworkingSockets()->CloseConnection(pCallback->m_hConn, 0, nullptr, false);

            if (_isServer)
            {
                _fromNetwork.Push({NetworkEventType::ClientDisconnected, connId});
            }
            else
            {
                _fromNetwork.Push({NetworkEventType::ConnectionFailure});
            }

            _connections.erase(connId);
            break;
        }
        default:
            break;
        }
    }

    // -------------------------------------------------------------------------
    // Network thread
    // -------------------------------------------------------------------------

    void SteamNetwork::NetworkLoop()
    {
        while (_running)
        {
            double now = GetTime();

            if (_isServer)
                ServerPing(now);
            else if (ClientTimeout(now))
                return;

            SteamNetworkingSockets()->RunCallbacks();
            ReceiveMessages();

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void SteamNetwork::ServerPing(double now)
    {
        if (now - _lastPingSend < 1.0)
            return;

        _lastPingSend = now;

        for (auto& conn: _connections | std::views::values)
            SendMessage(conn, PingPacket, 0);
    }

    bool SteamNetwork::ClientTimeout(double now)
    {
        if (!_connected || _serverConnection == k_HSteamNetConnection_Invalid)
            return false;
        if (now - _lastPingTime < PingTimeout)
            return false;

        _fromNetwork.Push({NetworkEventType::ServerDisconnected});
        SteamNetworkingSockets()->CloseConnection(_serverConnection, 0, nullptr, false);
        _serverConnection = k_HSteamNetConnection_Invalid;
        _running = false;
        return true;
    }

    void SteamNetwork::ReceiveMessages()
    {
        SteamNetworkingMessage_t* messages[64];
        int count = 0;

        if (_isServer)
            count = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(_pollGroup, messages, 64);
        else if (_serverConnection != k_HSteamNetConnection_Invalid)
            count = SteamNetworkingSockets()->ReceiveMessagesOnConnection(_serverConnection, messages, 64);

        for (int i = 0; i < count; i++)
        {
            SteamNetworkingMessage_t* steamMsg = messages[i];

            const uint8_t* data = static_cast<const uint8_t*>(steamMsg->m_pData);
            int dataLen = steamMsg->m_cbSize;

            // Ping packet — reset timeout timer, don't forward to game
            if (dataLen == 1 && data[0] == static_cast<uint8_t>(NetworkEventType::Ping))
            {
                _lastPingTime = GetTime();
                steamMsg->Release();
                continue;
            }

            NetworkMessage msg;
            msg.type = NetworkEventType::Message;
            msg.peerId = steamMsg->m_conn;
            msg.data.assign(data, data + dataLen);
            steamMsg->Release();

            _fromNetwork.Push(msg);
        }
    }

    void SteamNetwork::SendMessage(HSteamNetConnection conn, const std::vector<uint8_t>& data, uint32_t flags)
    {
        // Map ENet-style flags: bit 0 = reliable
        int steamFlags = (flags & 1) != 0 ? k_nSteamNetworkingSend_Reliable : k_nSteamNetworkingSend_Unreliable;

        SteamNetworkingSockets()->SendMessageToConnection(
                conn,
                data.data(),
                static_cast<uint32_t>(data.size()),
                steamFlags,
                nullptr);
    }

    // -------------------------------------------------------------------------
    // Public interface
    // -------------------------------------------------------------------------

    void SteamNetwork::SendToServer(const std::vector<uint8_t>& data, uint32_t flags)
    {
        if (loopbackPeer != nullptr)
        {
            NetworkMessage msg;
            msg.type = NetworkEventType::Message;
            msg.peerId = 0;
            msg.data = data;
            msg.flags = flags;
            loopbackPeer->loopbackMessages.push(std::move(msg));
            return;
        }

        if (_isServer || _serverConnection == k_HSteamNetConnection_Invalid)
            return;

        SendMessage(_serverConnection, data, flags);
    }

    void SteamNetwork::SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, uint32_t flags)
    {
        if (loopbackPeer != nullptr && peerId == 0)
        {
            NetworkMessage msg;
            msg.type = NetworkEventType::Message;
            msg.peerId = peerId;
            msg.data = data;
            msg.flags = flags;
            loopbackPeer->loopbackMessages.push(std::move(msg));
            return;
        }

        if (!_isServer)
            return;

        auto it = _connections.find(peerId);
        if (it == _connections.end())
            return;

        SendMessage(it->second, data, flags);
    }

    void SteamNetwork::Poll()
    {
        while (auto msg = _fromNetwork.Pop())
        {
            if (onMessageReceived)
                onMessageReceived(*msg);
        }

        while (!loopbackMessages.empty())
        {
            if (onMessageReceived)
                onMessageReceived(loopbackMessages.front());
            loopbackMessages.pop();
        }
    }

    bool SteamNetwork::Connected()
    {
        return _running && _connected;
    }

    void SteamNetwork::Clean()
    {
        _running = false;

        if (_networkThread.joinable())
            _networkThread.join();

        if (_isServer)
        {
            if (_pollGroup != k_HSteamNetPollGroup_Invalid)
            {
                SteamNetworkingSockets()->DestroyPollGroup(_pollGroup);
                _pollGroup = k_HSteamNetPollGroup_Invalid;
            }
            if (_listenSocket != k_HSteamListenSocket_Invalid)
            {
                SteamNetworkingSockets()->CloseListenSocket(_listenSocket);
                _listenSocket = k_HSteamListenSocket_Invalid;
            }
        }
        else if (_serverConnection != k_HSteamNetConnection_Invalid)
        {
            SteamNetworkingSockets()->CloseConnection(_serverConnection, 0, nullptr, false);
            _serverConnection = k_HSteamNetConnection_Invalid;
        }

        _connections.clear();

        if (_upnpMapped)
        {
            const std::string portStr = std::to_string(_port);
            UPNP_DeletePortMapping(
                    _upnpUrls.controlURL,
                    _upnpData.first.servicetype,
                    portStr.c_str(),
                    "UDP",
                    nullptr);
            _upnpMapped = false;
            SDL_Log("UPnP: Port mapping removed.");
        }

        if (_upnpDevList)
        {
            freeUPNPDevlist(_upnpDevList);
            _upnpDevList = nullptr;
        }

        FreeUPNPUrls(&_upnpUrls);
    }

    // =============================================================================
    // Stub implementation — Steamworks not available
    // =============================================================================

#else

    SteamNetwork::SteamNetwork(int /*port*/, bool /*localOnly*/)
    {
        SDL_Log("SteamNetwork: Steamworks support was not compiled into this build of Igneous. "
                "Enable it with -DIGNEOUS_STEAM=ON and provide the Steamworks SDK.");
    }

    SteamNetwork::SteamNetwork(int /*port*/, const std::string& /*ip*/)
    {
        SDL_Log("SteamNetwork: Steamworks support was not compiled into this build of Igneous. "
                "Enable it with -DIGNEOUS_STEAM=ON and provide the Steamworks SDK.");
    }

    SteamNetwork::SteamNetwork()
    {
        SDL_Log("SteamNetwork: Steamworks support was not compiled into this build of Igneous. "
                "Enable it with -DIGNEOUS_STEAM=ON and provide the Steamworks SDK.");
    }

    SteamNetwork::~SteamNetwork() = default;

    void SteamNetwork::SendToServer(const std::vector<uint8_t>& /*data*/, enet_uint32 /*flags*/)
    {
    }
    void SteamNetwork::SendToClient(uint32_t /*peerId*/, const std::vector<uint8_t>& /*data*/, enet_uint32 /*flags*/)
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