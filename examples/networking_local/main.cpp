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

    std::vector<uint8_t> MakeDemoPacket(const std::string& text, int value)
    {
        Engine::Serializer ser;
        ser.Write(static_cast<uint16_t>(kDemoPacket));
        ser.Write(text);
        ser.Write(static_cast<int32_t>(value));
        return ser.GetBytes();
    }
}

class NetworkingLocalScene : public Engine::Scene
{
  public:
    Engine::NetworkSessionFactory::LocalClientServer session{};
    Engine::PacketRouter serverRouter{};
    Engine::PacketRouter clientRouter{};
    std::deque<std::string> log;
    int counter = 0;

    void OnCreated() override
    {
        session = Engine::NetworkSessionFactory::CreateLocalClientServer();

        session.server->onMessageReceived = [this](const Engine::NetworkMessage& msg)
        {
            if (msg.type == Engine::NetworkEventType::ConnectionSuccess)
                log.push_front("server: connected");
            else if (msg.type == Engine::NetworkEventType::Message)
                serverRouter.DispatchMessage(msg.peerId, msg.data);
        };

        session.client->onMessageReceived = [this](const Engine::NetworkMessage& msg)
        {
            if (msg.type == Engine::NetworkEventType::ConnectionSuccess)
                log.push_front("client: connected");
            else if (msg.type == Engine::NetworkEventType::Message)
                clientRouter.DispatchMessage(msg.peerId, msg.data);
        };

        serverRouter.Connect(kDemoPacket, [this](uint32_t peerId, const std::vector<uint8_t>& data)
        {
            (void) peerId;
            Engine::Deserializer des(data);
            std::string text = des.ReadString();
            int value = des.ReadInt();
            log.push_front("server got: " + text + " (" + std::to_string(value) + ")");

            session.server->SendToClient(
                    Engine::NetworkPeerIds::Local,
                    MakeDemoPacket("ack", value),
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
        ImGui::Begin("LocalNetwork");
        ImGui::Text("Transport: in-process loopback via NetworkSessionFactory.");
        if (ImGui::Button("Client -> Server"))
        {
            counter++;
            session.client->SendToServer(
                    MakeDemoPacket("move", counter),
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
    return engine.Run<NetworkingLocalScene>("NetworkingLocal");
}
