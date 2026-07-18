#include "igneous/engine/Engine.hpp"
#include "igneous/input/Input.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

#include <algorithm>
#include <string>
#include <vector>

namespace
{
    struct FlashTimer
    {
        double remaining = 0.0;

        void Trigger(double duration = 0.5)
        {
            remaining = duration;
        }

        void Update(double delta)
        {
            remaining = std::max(0.0, remaining - delta);
        }

        bool IsActive() const
        {
            return remaining > 0.0;
        }
    };
}

class InputLayersScene : public Engine::Scene
{
  public:
    bool menuOpen = false;
    Engine::Vec2<float> player{320, 180};
    std::string lastMenuLayerEvent = "none";
    std::string lastGameplayLayerEvent = "none";
    FlashTimer menuFlash{};
    FlashTimer gameplayFlash{};

    void OnCreated() override
    {
        Engine::Input::AddInputLayer(Engine::Input::GameplayLayer, 0);
        Engine::Input::AddInputLayer(Engine::Input::UILayer, 1);
        Engine::Input::AddInputLayer(Engine::Input::MenuLayer, 2);
    }

    void Render() override
    {
        ImGui::Begin("Input Layers");
        ImGui::Text("Layers (high -> low): menu (2), ui (1), gameplay (0), _default (0)");
        ImGui::Text("Menu open: %s", menuOpen ? "yes" : "no");
        ImGui::Separator();

        ImGui::Text("Player position: (%.0f, %.0f)", player.x, player.y);
        ImGui::Text("WASD moves player when menu is closed.");
        ImGui::Text("Tab toggles menu. Escape closes menu or quits when menu is closed.");
        ImGui::Separator();

        ImGui::Text("Last menu-layer event: %s", lastMenuLayerEvent.c_str());
        ImGui::Text("Last gameplay-layer event: %s", lastGameplayLayerEvent.c_str());
        if (menuFlash.IsActive())
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.5f, 1.0f), "Menu layer edge detected");
        if (gameplayFlash.IsActive())
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.5f, 1.0f), "Gameplay layer edge detected");
        ImGui::Separator();

        ImGui::Text("Key handled state (W): %s", Engine::Input::IsKeyHandled(SDLK_W) ? "yes" : "no");
        ImGui::End();
    }

    void Update(double delta) override
    {
        menuFlash.Update(delta);
        gameplayFlash.Update(delta);
    }

    void LogMenuEvent(const std::string& message)
    {
        lastMenuLayerEvent = message;
        menuFlash.Trigger();
    }

    void LogGameplayEvent(const std::string& message)
    {
        lastGameplayLayerEvent = message;
        gameplayFlash.Trigger();
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        if (layer.Is(Engine::Input::MenuLayer))
        {
            if (menuOpen)
            {
                static const std::vector<SDL_Keycode> kMenuExceptions{SDLK_ESCAPE, SDLK_TAB};
                Engine::Input::HandleAllInputs(&kMenuExceptions, nullptr, nullptr, nullptr);

                if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE, false))
                {
                    menuOpen = false;
                    LogMenuEvent("Escape closed menu");
                }
                else if (Engine::Input::IsKeyJustPressed(SDLK_TAB, false))
                {
                    menuOpen = false;
                    LogMenuEvent("Tab closed menu");
                }
                else if (Engine::Input::IsKeyJustPressed(SDLK_W, false)
                    || Engine::Input::IsKeyJustPressed(SDLK_A, false)
                    || Engine::Input::IsKeyJustPressed(SDLK_S, false)
                    || Engine::Input::IsKeyJustPressed(SDLK_D, false))
                {
                    LogMenuEvent("Movement key consumed by menu layer");
                }
            }
            else if (Engine::Input::IsKeyJustPressed(SDLK_TAB, false))
            {
                menuOpen = true;
                LogMenuEvent("Tab opened menu");
            }
        }

        if (layer.Is(Engine::Input::GameplayLayer))
        {
            if (menuOpen)
                return;

            float speed = 4.0f;
            if (Engine::Input::IsKeyDown(SDLK_W))
                player.y -= speed;
            if (Engine::Input::IsKeyDown(SDLK_S))
                player.y += speed;
            if (Engine::Input::IsKeyDown(SDLK_A))
                player.x -= speed;
            if (Engine::Input::IsKeyDown(SDLK_D))
                player.x += speed;

            if (Engine::Input::IsKeyJustPressed(SDLK_W)
                || Engine::Input::IsKeyJustPressed(SDLK_A)
                || Engine::Input::IsKeyJustPressed(SDLK_S)
                || Engine::Input::IsKeyJustPressed(SDLK_D))
            {
                LogGameplayEvent("Gameplay consumed movement key");
            }

            if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE, false))
                Engine::Engine::Quit();
        }
    }
};

int main()
{
    Engine::Engine engine;
    return engine.Run<InputLayersScene>("Input Layers");
}
