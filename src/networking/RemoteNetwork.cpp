#include "igneous/networking/RemoteNetwork.hpp"

#include <chrono>
#include <SDL3/SDL_log.h>

#include "igneous/networking/NetworkEvents.hpp"

namespace Engine
{
    const std::vector<uint8_t> RemoteNetwork::PingPacket = {static_cast<uint8_t>(NetworkEventType::Ping)};

    // Returns seconds since Unix epoch using std::chrono.
    // UPnP (Godot-specific) is not implemented — add your own port mapping
    // solution here if needed (e.g. libnatpmp or miniupnpc).
    double RemoteNetwork::GetTime() const
    {
        using namespace std::chrono;
        return duration<double>(system_clock::now().time_since_epoch()).count();
    }

    RemoteNetwork::RemoteNetwork(int port, int peerCount, bool localOnly)
    {
        _isServer = true;
        _port = port;

        ENetAddress address{};
        address.port = static_cast<enet_uint16>(port);

        if (localOnly)
            enet_address_set_host_ip(&address, "127.0.0.1");
        else
            address.host = ENET_HOST_ANY;

        _host = enet_host_create(&address, peerCount, 2, 0, 0);
        if (!_host)
        {
            SDL_Log("Failed to create server.");
            _fromNetwork.Push({NetworkEventType::ConnectionFailure});
            return;
        }

        _running = true;
        _fromNetwork.Push({NetworkEventType::ConnectionSuccess});
        _networkThread = std::thread(&RemoteNetwork::NetworkLoop, this);
    }

    RemoteNetwork::RemoteNetwork(int port, const std::string& ip)
    {
        _isServer = false;

        _host = enet_host_create(nullptr, 1, 2, 0, 0);
        if (!_host)
        {
            SDL_Log("Failed to create client.");
            _fromNetwork.Push({NetworkEventType::ConnectionFailure});
            return;
        }

        ENetAddress address{};
        enet_address_set_host_ip(&address, ip.c_str());
        address.port = static_cast<enet_uint16>(port);

        _serverPeer = enet_host_connect(_host, &address, 2, 0);
        if (!_serverPeer)
        {
            SDL_Log("Failed to create peer.");
            enet_host_destroy(_host);
            _host = nullptr;
            _fromNetwork.Push({NetworkEventType::ConnectionFailure});
            return;
        }

        _running = true;
        _connecting = true;
        _networkThread = std::thread(&RemoteNetwork::NetworkLoop, this);
    }

    RemoteNetwork::RemoteNetwork()
    {
        _isServer = false;
        _running = true;
        _fromNetwork.Push({NetworkEventType::ConnectionSuccess});
    }

    RemoteNetwork::~RemoteNetwork()
    {
        RemoteNetwork::Clean();
    }

    void RemoteNetwork::NetworkLoop()
    {
        while (_running)
        {
            if (_connecting)
            {
                ENetEvent event{};
                int result = enet_host_service(_host, &event, 5000);

                if (result < 0)
                {
                    SDL_Log("ENet host service error.");
                    enet_host_destroy(_host);
                    _host = nullptr;
                    _fromNetwork.Push({NetworkEventType::ConnectionFailure});
                    return;
                }

                if (event.type == ENET_EVENT_TYPE_NONE)
                {
                    SDL_Log("Connection timed out — no response from server within 5000ms.");
                    enet_host_destroy(_host);
                    _host = nullptr;
                    _serverPeer = nullptr;
                    _fromNetwork.Push({NetworkEventType::ConnectionFailure});
                    return;
                }

                if (event.type == ENET_EVENT_TYPE_DISCONNECT)
                {
                    SDL_Log("Server actively refused connection.");
                    enet_host_destroy(_host);
                    _host = nullptr;
                    _serverPeer = nullptr;
                    _fromNetwork.Push({NetworkEventType::ConnectionFailure});
                    return;
                }

                if (event.type != ENET_EVENT_TYPE_CONNECT)
                {
                    SDL_Log("Unexpected event during connection: %d", static_cast<int>(event.type));
                    enet_host_destroy(_host);
                    _host = nullptr;
                    _serverPeer = nullptr;
                    _fromNetwork.Push({NetworkEventType::ConnectionFailure});
                    return;
                }

                _connecting = false;
                _lastPingTime = GetTime();
                _fromNetwork.Push({NetworkEventType::ConnectionSuccess});
            }
            else
            {
                double now = GetTime();

                if (_isServer)
                    ServerPing(now);
                else if (ClientTimeout(now))
                    return;

                ENetEvent event{};
                while (_host && enet_host_service(_host, &event, 1) > 0)
                {
                    if (_isServer)
                        HandleServerEvent(event);
                    else
                        HandleClientEvent(event);
                }

                if (_host)
                    enet_host_flush(_host);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void RemoteNetwork::ServerPing(double now)
    {
        if (now - _lastPingSend < 1.0)
            return;

        _lastPingSend = now;

        for (auto& [id, peer]: _peerLookup)
        {
            ENetPacket* packet = enet_packet_create(PingPacket.data(), PingPacket.size(), 0);
            enet_peer_send(peer, 0, packet);
        }
    }

    bool RemoteNetwork::ClientTimeout(double now)
    {
        if (!_serverPeer)
            return false;
        if (now - _lastPingTime < PingTimeout)
            return false;

        _fromNetwork.Push({NetworkEventType::ServerDisconnected});
        enet_peer_reset(_serverPeer);
        _serverPeer = nullptr;
        _running = false;
        return true;
    }

    void RemoteNetwork::HandleServerEvent(const ENetEvent& event)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            uint32_t peerId = _nextPeerId++;
            _peerLookup[peerId] = event.peer;
            _peerIdLookup[event.peer] = peerId;
            _fromNetwork.Push({NetworkEventType::ClientConnected, peerId});
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            auto it = _peerIdLookup.find(event.peer);
            if (it == _peerIdLookup.end())
                break;

            uint32_t peerId = it->second;
            _peerLookup.erase(peerId);
            _peerIdLookup.erase(it);
            _fromNetwork.Push({NetworkEventType::ClientDisconnected, peerId});
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            auto it = _peerIdLookup.find(event.peer);
            if (it == _peerIdLookup.end())
            {
                enet_packet_destroy(event.packet);
                break;
            }

            NetworkMessage msg;
            msg.type = NetworkEventType::Message;
            msg.peerId = it->second;
            msg.data.assign(event.packet->data, event.packet->data + event.packet->dataLength);
            enet_packet_destroy(event.packet);
            _fromNetwork.Push(msg);
            break;
        }
        default:
            break;
        }
    }

    void RemoteNetwork::HandleClientEvent(const ENetEvent& event)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
        {
            if (event.packet->dataLength == 1 && event.packet->data[0] == static_cast<uint8_t>(NetworkEventType::Ping))
            {
                _lastPingTime = GetTime();
                enet_packet_destroy(event.packet);
                break;
            }

            NetworkMessage msg;
            msg.type = NetworkEventType::Message;
            msg.peerId = event.peer->connectID;
            msg.data.assign(event.packet->data, event.packet->data + event.packet->dataLength);
            enet_packet_destroy(event.packet);
            _fromNetwork.Push(msg);
            break;
        }
        default:
            break;
        }
    }

    void RemoteNetwork::Poll()
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

    bool RemoteNetwork::Connected()
    {
        return _running && !_connecting;
    }

    void RemoteNetwork::SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, enet_uint32 flags)
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

        auto it = _peerLookup.find(peerId);
        if (it == _peerLookup.end())
            return;

        ENetPacket* packet = enet_packet_create(data.data(), data.size(), flags);
        enet_peer_send(it->second, 0, packet);
    }

    void RemoteNetwork::SendToServer(const std::vector<uint8_t>& data, enet_uint32 flags)
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

        if (_isServer || !_serverPeer)
            return;

        ENetPacket* packet = enet_packet_create(data.data(), data.size(), flags);
        enet_peer_send(_serverPeer, 0, packet);
    }

    void RemoteNetwork::Clean()
    {
        _running = false;

        if (_networkThread.joinable())
            _networkThread.join();

        if (_host)
        {
            enet_host_destroy(_host);
            _host = nullptr;
        }

        _peerLookup.clear();
        _peerIdLookup.clear();
        _nextPeerId = 1;

        // UPnP port cleanup would go here if using miniupnpc or libnatpmp.
    }
}