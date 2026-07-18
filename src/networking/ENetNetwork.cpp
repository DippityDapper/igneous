#include "igneous/networking/ENetNetwork.hpp"

#include "upnpcommands.h"

#include <chrono>
#include <SDL3/SDL_log.h>

#include "igneous/networking/NetworkEvents.hpp"
#include "igneous/networking/NetworkPeerIds.hpp"

namespace Engine
{
    double ENetNetwork::GetTime()
    {
        using namespace std::chrono;
        return duration<double>(system_clock::now().time_since_epoch()).count();
    }

    enet_uint32 ENetNetwork::ToEnetFlags(TransportType flags)
    {
        return flags == TransportType::Reliable ? ENET_PACKET_FLAG_RELIABLE : 0;
    }

    ENetNetwork::~ENetNetwork()
    {
        Clean();
    }

    void ENetNetwork::Connect(int listenPort, int peerCount, bool localOnly)
    {
        isServer = true;
        clientWatchdog.enabled = false;
        port = listenPort;

        ENetAddress address{};
        address.port = static_cast<enet_uint16>(listenPort);
        enet_address_set_host_ip(&address, localOnly ? "127.0.0.1" : "0.0.0.0");

        host = enet_host_create(&address, peerCount, 2, 0, 0);
        if (!host)
        {
            SDL_Log("ENetNetwork: Failed to create server.");
            loopback.Forward({NetworkEventType::ConnectionFailure});
            return;
        }

        if (!localOnly)
        {
            upnpThread = std::thread([this, listenPort]()
            {
                MapPort(listenPort);
            });
        }

        running = true;
        networkThread = std::thread(&ENetNetwork::NetworkLoop, this);
        loopback.Forward({NetworkEventType::ConnectionSuccess});
    }

    void ENetNetwork::Connect(int listenPort, const std::string& ip)
    {
        isServer = false;
        clientWatchdog.enabled = false;

        host = enet_host_create(nullptr, 1, 2, 0, 0);
        if (!host)
        {
            SDL_Log("ENetNetwork: Failed to create client.");
            loopback.Forward({NetworkEventType::ConnectionFailure});
            return;
        }

        ENetAddress address{};
        enet_address_set_host_ip(&address, ip.c_str());
        address.port = static_cast<enet_uint16>(listenPort);

        serverPeer = enet_host_connect(host, &address, 2, 0);
        if (!serverPeer)
        {
            SDL_Log("ENetNetwork: Failed to create peer.");
            enet_host_destroy(host);
            host = nullptr;
            loopback.Forward({NetworkEventType::ConnectionFailure});
            return;
        }

        running = true;
        connecting = true;
        networkThread = std::thread(&ENetNetwork::NetworkLoop, this);
        loopback.Forward({NetworkEventType::ConnectionSuccess});
    }

    void ENetNetwork::Connect()
    {
        isServer = false;
        running = true;
        clientWatchdog.enabled = false;
        loopback.Forward({NetworkEventType::ConnectionSuccess});
    }

    void ENetNetwork::MapPort(int listenPort)
    {
        int error = 0;
        upnpDevList = upnpDiscover(2000, nullptr, nullptr, UPNP_LOCAL_PORT_ANY, 0, 2, &error);
        if (!upnpDevList)
        {
            SDL_Log("ENetNetwork UPnP: Discovery failed (error %d).", error);
            return;
        }

        char lanAddr[64] = {};
        char wanAddr[64] = {};
        int igdStatus = UPNP_GetValidIGD(upnpDevList, &upnpUrls, &upnpData, lanAddr, sizeof(lanAddr), wanAddr, sizeof(wanAddr));
        if (igdStatus != 1)
        {
            SDL_Log("ENetNetwork UPnP: No valid gateway found (IGD status %d).", igdStatus);
            return;
        }

        const std::string portStr = std::to_string(listenPort);
        int mapResult = UPNP_AddPortMapping(
                upnpUrls.controlURL,
                upnpData.first.servicetype,
                portStr.c_str(),
                portStr.c_str(),
                lanAddr,
                "Igneous",
                "UDP",
                nullptr,
                "0");

        if (mapResult == UPNPCOMMAND_SUCCESS)
        {
            upnpMapped = true;
            port = listenPort;

            char externalIP[40] = {};
            UPNP_GetExternalIPAddress(upnpUrls.controlURL, upnpData.first.servicetype, externalIP);
            SDL_Log("ENetNetwork UPnP: Port %d mapped successfully. External IP: %s", listenPort, externalIP);
        }
        else
        {
            SDL_Log("ENetNetwork UPnP: Port mapping failed with code %d", mapResult);
        }
    }

    void ENetNetwork::NetworkLoop()
    {
        while (running)
        {
            ProcessOutbound();

            if (connecting)
            {
                ENetEvent event{};
                int result = enet_host_service(host, &event, 5000);

                if (result < 0)
                {
                    SDL_Log("ENetNetwork: host service error.");
                    enet_host_destroy(host);
                    host = nullptr;
                    fromNetwork.Push({NetworkEventType::ConnectionFailure});
                    return;
                }

                if (event.type == ENET_EVENT_TYPE_NONE)
                {
                    SDL_Log("ENetNetwork: connection timed out.");
                    enet_host_destroy(host);
                    host = nullptr;
                    serverPeer = nullptr;
                    fromNetwork.Push({NetworkEventType::ConnectionFailure});
                    return;
                }

                if (event.type == ENET_EVENT_TYPE_DISCONNECT)
                {
                    SDL_Log("ENetNetwork: server refused connection.");
                    enet_host_destroy(host);
                    host = nullptr;
                    serverPeer = nullptr;
                    fromNetwork.Push({NetworkEventType::ConnectionFailure});
                    return;
                }

                if (event.type != ENET_EVENT_TYPE_CONNECT)
                {
                    SDL_Log("ENetNetwork: unexpected event during connection: %d", static_cast<int>(event.type));
                    enet_host_destroy(host);
                    host = nullptr;
                    serverPeer = nullptr;
                    fromNetwork.Push({NetworkEventType::ConnectionFailure});
                    return;
                }

                connecting = false;
                fromNetwork.Push({NetworkEventType::ConnectionSuccess});
                const double connectedAt = GetTime();
                clientWatchdog.Reset(connectedAt);
                clientWatchdog.enabled = serverPeer != nullptr;
            }
            else
            {
                const double now = GetTime();

                if (isServer)
                {
                    ServerPing(now);
                    DisconnectStalePeers(now);
                }
                else if (HandleClientTimeout(now))
                {
                    return;
                }

                ENetEvent event{};
                while (host && enet_host_service(host, &event, 1) > 0)
                {
                    if (isServer)
                        HandleServerEvent(event);
                    else
                        HandleClientEvent(event);
                }

                if (host)
                    enet_host_flush(host);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void ENetNetwork::ProcessOutbound()
    {
        while (auto op = outbound.Pop())
        {
            switch (op->kind)
            {
            case EnetOutboundKind::SendToServer:
                SendToServerOnNetworkThread(op->data, op->transport);
                break;
            case EnetOutboundKind::SendToClient:
                SendToClientOnNetworkThread(op->peerId, op->data, op->transport);
                break;
            case EnetOutboundKind::DisconnectPeer:
                DisconnectPeerOnNetworkThread(op->peerId);
                break;
            case EnetOutboundKind::GracefulClientDisconnect:
                GracefulClientDisconnectOnNetworkThread();
                break;
            }
        }
    }

    void ENetNetwork::ServerPing(double now)
    {
        if (!serverPeers.TryConsumePingSend(now))
            return;

        for (const auto& [peerId, peer]: peerLookup)
        {
            (void) peerId;
            ENetPacket* packet = enet_packet_create(
                    ClientConnectionWatchdog::PingPacket.data(),
                    ClientConnectionWatchdog::PingPacket.size(),
                    ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
        }
    }

    bool ENetNetwork::HandleClientTimeout(double now)
    {
        if (!clientWatchdog.HasTimedOut(now))
            return false;

        SDL_Log("ENetNetwork: server connection timed out.");
        fromNetwork.Push({NetworkEventType::ServerDisconnected});

        if (serverPeer)
            enet_peer_reset(serverPeer);
        serverPeer = nullptr;
        clientWatchdog.enabled = false;
        running = false;
        return true;
    }

    void ENetNetwork::DisconnectStalePeers(double now)
    {
        for (uint32_t peerId: serverPeers.CollectStalePeerIds(now))
            DisconnectStalePeer(peerId);
    }

    void ENetNetwork::DisconnectStalePeer(uint32_t peerId)
    {
        if (!serverPeers.RemovePeer(peerId))
            return;

        fromNetwork.Push({NetworkEventType::ClientDisconnected, peerId});

        auto it = peerLookup.find(peerId);
        if (it != peerLookup.end())
        {
            if (it->second)
                enet_peer_reset(it->second);
            peerLookup.erase(it);
        }
    }

    void ENetNetwork::ConfigureServerPeer(ENetPeer* peer)
    {
        if (!peer)
            return;

        const uint32_t timeoutMs = static_cast<uint32_t>(serverPeers.timeoutSeconds * 1000.0);
        enet_peer_ping_interval(peer, EnetPeerPingIntervalMs);
        enet_peer_timeout(peer, 32, 5000, timeoutMs);
    }

    void ENetNetwork::HandleServerEvent(const ENetEvent& event)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            const uint32_t peerId = NetworkPeerIds::FromEnetPeer(event.peer->connectID);
            auto existing = peerLookup.find(peerId);
            if (existing != peerLookup.end())
            {
                if (existing->second)
                    enet_peer_reset(existing->second);
                peerLookup.erase(existing);
            }

            fromNetwork.Push({NetworkEventType::ClientConnected, peerId});
            peerLookup[peerId] = event.peer;
            ConfigureServerPeer(event.peer);
            serverPeers.TrackPeer(peerId, GetTime());
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            const uint32_t peerId = NetworkPeerIds::FromEnetPeer(event.peer->connectID);
            serverPeers.RemovePeer(peerId);
            if (!peerLookup.erase(peerId))
                break;

            fromNetwork.Push({NetworkEventType::ClientDisconnected, peerId});
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            const uint32_t peerId = NetworkPeerIds::FromEnetPeer(event.peer->connectID);
            serverPeers.MarkActivity(peerId, GetTime());

            NetworkMessage msg;
            msg.type = NetworkEventType::Message;
            msg.peerId = peerId;
            msg.data.assign(event.packet->data, event.packet->data + event.packet->dataLength);
            enet_packet_destroy(event.packet);
            fromNetwork.Push(std::move(msg));
            break;
        }
        default:
            break;
        }
    }

    void ENetNetwork::HandleClientEvent(const ENetEvent& event)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
        {
            if (event.packet->dataLength == 1 && event.packet->data[0] == static_cast<uint8_t>(NetworkEventType::Ping))
            {
                clientWatchdog.MarkActivity(GetTime());
                enet_packet_destroy(event.packet);
                break;
            }

            clientWatchdog.MarkActivity(GetTime());

            NetworkMessage msg;
            msg.type = NetworkEventType::Message;
            msg.peerId = NetworkPeerIds::FromEnetPeer(event.peer->connectID);
            msg.data.assign(event.packet->data, event.packet->data + event.packet->dataLength);
            enet_packet_destroy(event.packet);
            fromNetwork.Push(std::move(msg));
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            fromNetwork.Push({NetworkEventType::ServerDisconnected});
            serverPeer = nullptr;
            clientWatchdog.enabled = false;
            running = false;
            break;
        }
        default:
            break;
        }
    }

    void ENetNetwork::SendToServerOnNetworkThread(const std::vector<uint8_t>& data, TransportType flags)
    {
        if (isServer || !serverPeer)
            return;

        ENetPacket* packet = enet_packet_create(data.data(), data.size(), ToEnetFlags(flags));
        enet_peer_send(serverPeer, 0, packet);
    }

    void ENetNetwork::SendToClientOnNetworkThread(uint32_t peerId, const std::vector<uint8_t>& data, TransportType flags)
    {
        if (!isServer)
            return;

        auto it = peerLookup.find(peerId);
        if (it == peerLookup.end() || !it->second)
            return;

        ENetPacket* packet = enet_packet_create(data.data(), data.size(), ToEnetFlags(flags));
        enet_peer_send(it->second, 0, packet);
    }

    void ENetNetwork::DisconnectPeerOnNetworkThread(uint32_t peerId)
    {
        if (!isServer)
            return;

        auto it = peerLookup.find(peerId);
        if (it == peerLookup.end() || !it->second)
            return;

        if (host)
            enet_host_flush(host);
        enet_peer_disconnect(it->second, 0);
        peerLookup.erase(it);
    }

    void ENetNetwork::GracefulClientDisconnectOnNetworkThread()
    {
        if (isServer || !serverPeer)
            return;

        if (host)
            enet_host_flush(host);
        enet_peer_disconnect(serverPeer, 0);
        if (host)
            enet_host_flush(host);
    }

    void ENetNetwork::SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, TransportType flags)
    {
        if (loopback.IsLinked() && peerId == NetworkPeerIds::Local)
        {
            loopback.Forward({NetworkEventType::Message, peerId, data});
            return;
        }

        outbound.Push({
                EnetOutboundKind::SendToClient,
                peerId,
                data,
                flags,
        });
    }

    void ENetNetwork::SendToServer(const std::vector<uint8_t>& data, TransportType flags)
    {
        if (loopback.IsLinked())
        {
            loopback.Forward({NetworkEventType::Message, NetworkPeerIds::Local, data});
            return;
        }

        outbound.Push({
                EnetOutboundKind::SendToServer,
                0,
                data,
                flags,
        });
    }

    void ENetNetwork::DisconnectPeer(uint32_t peerId)
    {
        outbound.Push({EnetOutboundKind::DisconnectPeer, peerId});
    }

    void ENetNetwork::Poll()
    {
        while (auto msg = fromNetwork.Pop())
        {
            if (onMessageReceived)
                onMessageReceived(*msg);
        }
    }

    bool ENetNetwork::Connected()
    {
        return running && !connecting;
    }

    void ENetNetwork::Clean()
    {
        if (!isServer && running && serverPeer)
        {
            outbound.Push({EnetOutboundKind::GracefulClientDisconnect});
            running = false;
            if (networkThread.joinable())
                networkThread.join();
        }
        else
        {
            running = false;
            if (networkThread.joinable())
                networkThread.join();
        }

        clientWatchdog.enabled = false;

        if (upnpThread.joinable())
            upnpThread.join();

        if (host)
        {
            enet_host_destroy(host);
            host = nullptr;
        }

        peerLookup.clear();
        serverPeer = nullptr;
        connecting = false;

        if (upnpMapped)
        {
            const std::string portStr = std::to_string(port);
            UPNP_DeletePortMapping(upnpUrls.controlURL, upnpData.first.servicetype, portStr.c_str(), "UDP", nullptr);
            upnpMapped = false;
        }

        if (upnpDevList)
        {
            freeUPNPDevlist(upnpDevList);
            upnpDevList = nullptr;
        }

        FreeUPNPUrls(&upnpUrls);
        loopback.Clear();
    }
}
