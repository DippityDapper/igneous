#include "igneous/engine/Engine.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/networking/NetworkEvents.hpp"
#include "igneous/networking/NetworkPeerIds.hpp"
#include "igneous/networking/NetworkSessionFactory.hpp"
#include "igneous/networking/PacketRouter.hpp"
#include "igneous/networking/PacketTypes.hpp"
#include "igneous/networking/Serializer.hpp"

#ifdef IGNEOUS_STEAM_ENABLED
#include "igneous/networking/SteamBootstrap.hpp"
#endif

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

#include <deque>
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

class NetworkingSteamScene : public Engine::Scene
{
  public:
    Engine::NetworkSessionFactory::SteamClientServer session{};
    Engine::PacketRouter serverRouter{};
    Engine::PacketRouter clientRouter{};
    std::deque<std::string> log;
    bool steamEnabled = false;

    void OnCreated() override
    {
#ifdef IGNEOUS_STEAM_ENABLED
        if (!Engine::SteamBootstrap::IsInitialized())
        {
            steamEnabled = false;
            log.push_front("Steam init failed. Is the Steam client running?");
            return;
        }

        steamEnabled = true;
        session = Engine::NetworkSessionFactory::CreateSteamClientServer();
#else
        steamEnabled = false;
        log.push_front("Steam disabled. Rebuild with -DIGNEOUS_STEAM=ON.");
        return;
#endif

        session.server->onMessageReceived = [this](const Engine::NetworkMessage& msg)
        {
            if (msg.type == Engine::NetworkEventType::ConnectionSuccess)
                log.push_front("steam server: P2P listen ready");
            else if (msg.type == Engine::NetworkEventType::Message)
                serverRouter.DispatchMessage(msg.peerId, msg.data);
        };

        session.client->onMessageReceived = [this](const Engine::NetworkMessage& msg)
        {
            if (msg.type == Engine::NetworkEventType::ConnectionSuccess)
                log.push_front("steam client: loopback connected");
            else if (msg.type == Engine::NetworkEventType::Message)
                clientRouter.DispatchMessage(msg.peerId, msg.data);
        };

        serverRouter.Connect(kDemoPacket, [this](uint32_t peerId, const std::vector<uint8_t>& data)
        {
            (void) peerId;
            Engine::Deserializer des(data);
            log.push_front("server got: " + des.ReadString());
            session.server->SendToClient(
                    Engine::NetworkPeerIds::Local,
                    MakeDemoPacket("steam-ack"),
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
        if (!steamEnabled)
            return;

        session.server->Poll();
        session.client->Poll();
    }

    void Render() override
    {
        ImGui::Begin("SteamNetwork");
        if (!steamEnabled)
        {
            ImGui::TextWrapped("Steamworks was not compiled into this build.");
            ImGui::Text("cmake -B build -DIGNEOUS_STEAM=ON");
        }
        else
        {
            ImGui::Text("P2P listen-server with loopback client.");
            if (ImGui::Button("Client -> Server"))
            {
                session.client->SendToServer(
                        MakeDemoPacket("steam-ping"),
                        Engine::TransportType::Reliable);
            }
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
        if (!steamEnabled)
            return;

        session.server->Clean();
        session.client->Clean();
    }
};

int main()
{
#ifdef IGNEOUS_STEAM_ENABLED
    Engine::SteamBootstrap::Init();
#endif

    Engine::Engine engine;
    return engine.Run<NetworkingSteamScene>("NetworkingSteam");
}
