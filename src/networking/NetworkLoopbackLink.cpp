#include "igneous/networking/NetworkLoopbackLink.hpp"

#include "igneous/networking/NetworkInterface.hpp"

namespace Engine
{
    void NetworkLoopbackLink::Forward(const NetworkMessage& message)
    {
        if (peer && peer->onMessageReceived)
            peer->onMessageReceived(message);
    }
}
