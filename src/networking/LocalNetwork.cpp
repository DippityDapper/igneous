#include "igneous/networking/LocalNetwork.hpp"

#include "igneous/networking/NetworkPeerIds.hpp"

namespace Engine
{
    void LocalNetwork::Connect(bool server)
    {
        isServer = server;
        loopback.Forward({NetworkEventType::ConnectionSuccess});
    }

    void LocalNetwork::SendToServer(const std::vector<uint8_t>& data, TransportType flags)
    {
        (void) flags;
        if (isServer)
            return;

        loopback.Forward({
                NetworkEventType::Message,
                NetworkPeerIds::Local,
                data,
        });
    }

    void LocalNetwork::SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, TransportType flags)
    {
        (void) flags;
        if (!isServer)
            return;

        loopback.Forward({
                NetworkEventType::Message,
                peerId,
                data,
        });
    }

    void LocalNetwork::Poll()
    {
    }

    bool LocalNetwork::Connected()
    {
        return loopback.IsLinked();
    }

    void LocalNetwork::Clean()
    {
        loopback.Clear();
    }
}
