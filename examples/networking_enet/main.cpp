#include "igneous/engine/Engine.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/networking/NetworkEvents.hpp"
#include "igneous/networking/NetworkPeerIds.hpp"
#include "igneous/networking/NetworkSessionFactory.hpp"
#include "igneous/networking/PacketRouter.hpp"
#include "igneous/networking/PacketTypes.hpp"
#include "igneous/networking/Serializer.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

#include <deque>
#include <memory>
#include <string>

namespace
{
    constexpr Engine::PacketType kDemoPacket = Engine::PacketType::ChatMessagePacket;

    std::vector<uint8_t> MakeDemoPacket(const std::string& text)
    {
        Engine::Serializer ser;
        ser.Write(static_cast<uint16_t>(kDemoPacket));
        ser.Write(text);
        return ser.GetBytes();
    }
}

class NetworkingEnetScene : public Engine::Scene
{
  public:
    Engine::NetworkSessionFactory::EnetClientServer session{};
    Engine::PacketRouter serverRouter{};
    Engine::PacketRouter clientRouter{};
    std::deque<std::string> log;

    void OnCreated() override
    {
        session = Engine::NetworkSessionFactory::CreateEnetClientServer(true);

        session.server->onMessageReceived = [this](const Engine::NetworkMessage& msg)
        {
            switch (msg.type)
            {
            case Engine::NetworkEventType::ConnectionSuccess:
                log.push_front("server: listening on 127.0.0.1:" + std::to_string(Engine::NetworkSessionFactory::EnetPort));
                break;
            case Engine::NetworkEventType::Message:
                serverRouter.DispatchMessage(msg.peerId, msg.data);
                break;
            default:
                break;
            }
        };

        session.client->onMessageReceived = [this](const Engine::NetworkMessage& msg)
        {
            switch (msg.type)
            {
            case Engine::NetworkEventType::ConnectionSuccess:
                log.push_front("client: loopback connected");
                break;
            case Engine::NetworkEventType::Message:
                clientRouter.DispatchMessage(msg.peerId, msg.data);
                break;
            default:
                break;
            }
        };

        serverRouter.Connect(kDemoPacket, [this](uint32_t peerId, const std::vector<uint8_t>& data)
        {
            (void) peerId;
            Engine::Deserializer des(data);
            std::string text = des.ReadString();
            log.push_front("server got: " + text);
            session.server->SendToClient(
                    Engine::NetworkPeerIds::Local,
                    MakeDemoPacket("enet-ack"),
                    Engine::TransportType::Reliable);
        });

        clientRouter.Connect(kDemoPacket, [this](uint32_t peerId, const std::vector<uint8_t>& data)
        {
            (void) peerId;
            Engine::Deserializer des(data);
            log.push_front("client got: " + des.ReadString());
        });

        session.server->Poll();
        session.client->Poll();
    }

    void Update(double delta) override
    {
        (void) delta;
        session.server->Poll();
        session.client->Poll();
    }

    void Render() override
    {
        ImGui::Begin("ENetNetwork");
        ImGui::Text("Listen-server demo on localhost:%d.", Engine::NetworkSessionFactory::EnetPort);
        ImGui::Text("Server + loopback client share one process.");
        if (ImGui::Button("Client -> Server"))
        {
            session.client->SendToServer(
                    MakeDemoPacket("enet-ping"),
                    Engine::TransportType::Reliable);
        }

        ImGui::Separator();
        for (const auto& line: log)
            ImGui::TextUnformatted(line.c_str());
        ImGui::End();
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;
        if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE))
            Engine::Engine::Quit();
    }

    void OnDestroyed() override
    {
        session.server->Clean();
        session.client->Clean();
    }
};

int main()
{
    Engine::Engine engine;
    return engine.Run<NetworkingEnetScene>("NetworkingEnet");
}
