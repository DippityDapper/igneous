#pragma once
#include <memory>

#include "igneous/networking/NetworkInterface.hpp"

namespace Engine
{
    class NetworkManager
    {
    public:
        static void CreateLocalNetwork(std::unique_ptr<NetworkInterface>& outServer, std::unique_ptr<NetworkInterface>& outClient);
        static std::unique_ptr<NetworkInterface> CreateServer(int port, int maxClients, bool localOnly = false);
        static std::unique_ptr<NetworkInterface> CreateClient(int port, const std::string& ip);

        template<typename T>
        static void BindMessageHandler(NetworkInterface* networkInterface, T* instance, void (T::*callback)(const NetworkMessage&));
    };

    template<typename T>
    void NetworkManager::BindMessageHandler(NetworkInterface *networkInterface, T *instance, void(T::*callback)(const NetworkMessage &))
    {
        networkInterface->onMessageReceived = [instance, callback](const NetworkMessage& message)
        {
            (instance->*callback)(message);
        };
    }
}
