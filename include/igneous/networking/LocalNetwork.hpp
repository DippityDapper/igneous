#pragma once

#include "igneous/networking/ILoopbackNetwork.hpp"
#include "igneous/networking/NetworkInterface.hpp"

namespace Engine
{

    class LocalNetwork : public NetworkInterface, public ILoopbackNetwork
    {
      private:
        NetworkLoopbackLink loopback{};
        bool isServer = false;

      public:
        LocalNetwork() = default;

        ~LocalNetwork() override = default;

        void Connect(bool server);

        NetworkLoopbackLink& GetLoopback() override
        {
            return loopback;
        }

        void SendToServer(const std::vector<uint8_t>& data, TransportType flags) override;

        void SendToClient(uint32_t peerId, const std::vector<uint8_t>& data, TransportType flags) override;

        void Poll() override;

        bool Connected() override;

        void Clean() override;
    };
}
