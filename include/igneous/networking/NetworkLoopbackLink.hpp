#pragma once

#include "igneous/networking/NetworkEvents.hpp"

namespace Engine
{

    class NetworkInterface;

    class NetworkLoopbackLink
    {
      private:
        NetworkInterface* peer = nullptr;

      public:
        bool IsLinked() const
        {
            return peer != nullptr;
        }

        void SetPeer(NetworkInterface* _peer)
        {
            peer = _peer;
        }

        void Clear()
        {
            peer = nullptr;
        }

        void Forward(const NetworkMessage& message);
    };
}
