#include "igneous/networking/LocalNetwork.hpp"

namespace Engine
{
    LocalNetwork::LocalNetwork(bool _isServer)
    {
        isServer = _isServer;
        NetworkMessage msg{NetworkEventType::ConnectionSuccess};
        loopbackMessages.emplace(msg);
    }

    void LocalNetwork::Poll()
    {
        while (!loopbackMessages.empty() && onMessageReceived)
        {
            onMessageReceived(loopbackMessages.front());
            loopbackMessages.pop();
        }
    }

    bool LocalNetwork::Connected()
    {
        return loopbackPeer != nullptr;
    }

    void LocalNetwork::Clean()
    {
    }

    void LocalNetwork::SendToServer(const std::vector<uint8_t>& data, uint32_t flags)
    {
        if (isServer)
            return;

        NetworkMessage msg;
        msg.type = NetworkEventType::Message;
        msg.peerId = 0;
        msg.data = data;
        msg.flags = flags;
        loopbackPeer->loopbackMessages.push(msg);
    }

    void LocalNetwork::SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, uint32_t flags)
    {
        if (!isServer)
            return;

        NetworkMessage msg;
        msg.type = NetworkEventType::Message;
        msg.peerId = peerId;
        msg.data = data;
        msg.flags = flags;
        loopbackPeer->loopbackMessages.push(msg);
    }
}
