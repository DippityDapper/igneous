#include "igneous/networking/LocalNetwork.hpp"

#include "SDL3/SDL_log.h"

namespace Engine
{
    void LocalNetwork::Poll()
    {
        while (!incomingMessages.empty() && onMessageReceived)
        {
            onMessageReceived(incomingMessages.front());
            incomingMessages.pop();
        }
    }

    bool LocalNetwork::Connected()
    {
        return networkPeer != nullptr;
    }

    void LocalNetwork::SetPeer(LocalNetwork* peer)
    {
        networkPeer = peer;
    }

    void LocalNetwork::SendToServer(const std::vector<uint8_t>& data, enet_uint32 flags)
    {
        NetworkMessage msg;
        msg.type = NetworkEventType::Message;
        msg.peer = nullptr;
        msg.data = data;
        msg.flags = flags;
        networkPeer->incomingMessages.push(msg);
    }

    void LocalNetwork::SendToClient(ENetPeer* peer, const std::vector<uint8_t>& data, enet_uint32 flags)
    {
        NetworkMessage msg;
        msg.type = NetworkEventType::Message;
        msg.peer = peer;
        msg.data = data;
        msg.flags = flags;
        networkPeer->incomingMessages.push(msg);
    }
}
