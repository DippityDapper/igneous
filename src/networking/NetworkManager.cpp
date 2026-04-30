#include "igneous/networking/NetworkManager.hpp"

#include "igneous/networking/LocalNetwork.hpp"

namespace Engine
{
    void NetworkManager::CreateLocalNetwork(std::unique_ptr<NetworkInterface>& outServer, std::unique_ptr<NetworkInterface>& outClient)
    {
        auto serverNet = std::make_unique<LocalNetwork>();
        auto clientNet = std::make_unique<LocalNetwork>();

        serverNet->SetPeer(clientNet.get());
        clientNet->SetPeer(serverNet.get());

        outServer = std::move(serverNet);
        outClient = std::move(clientNet);
    }

    std::unique_ptr<NetworkInterface> NetworkManager::CreateServer(int port, int maxClients, bool localOnly)
    {
        return std::make_unique<RemoteNetwork>(port, maxClients, localOnly);
    }

    std::unique_ptr<NetworkInterface> NetworkManager::CreateClient(int port, const std::string& ip)
    {
        return std::make_unique<RemoteNetwork>(port, ip);
    }
}
