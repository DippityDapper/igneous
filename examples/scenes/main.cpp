#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Vec2.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/resources/Sprite.hpp"
#include "igneous/scenes/SceneManager.hpp"

#include "imgui.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_render.h"

class MenuScene : public Engine::Scene
{
  public:
    void Render() override
    {
        ImGui::Begin("Menu scene");
        ImGui::Text("Press Enter to load the game scene.");
        ImGui::Text("Press Escape to quit.");
        ImGui::End();
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;
        if (Engine::Input::IsKeyJustPressed(SDLK_RETURN) && root)
            root->LoadScene("Game");
        if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE))
            Engine::Engine::Quit();
    }
};

class GameScene : public Engine::Scene
{
  public:
    Engine::Vec2<float> pos{320, 180};
    std::unique_ptr<Engine::Sprite> sprite;

    void OnCreated() override
    {
        sprite = std::make_unique<Engine::Sprite>(pos, "assets/sprites/crew_1.png");
        sprite->centered = true;
    }

    void Render() override
    {
        ImGui::Begin("Game scene");
        ImGui::Text("Press Backspace to return to the menu.");
        ImGui::End();
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;
        if (Engine::Input::IsKeyJustPressed(SDLK_BACKSPACE) && root)
            root->LoadScene("Menu");
        if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE))
            Engine::Engine::Quit();
    }
};

class ScenesBootstrap : public Engine::Scene
{
  public:
    void OnCreated() override
    {
        if (!root)
            return;
        root->AddScene<MenuScene>("Menu", "ui", true, false);
        root->AddScene<GameScene>("Game", "game", false, false);
    }

    void Update(double delta) override
    {
        (void) delta;
        static bool removed = false;
        if (!removed && root)
        {
            root->RemoveScene("Bootstrap");
            removed = true;
        }
    }
};

int main()
{
    Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_NEAREST);
    Engine::Engine engine;
    return engine.Run<ScenesBootstrap>("Bootstrap");
}
