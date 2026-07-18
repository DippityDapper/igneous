#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Vec2.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/resources/Sprite.hpp"

#include "imgui.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_render.h"

class StaticSpriteScene : public Engine::Scene
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
        ImGui::Begin("Static sprite");
        ImGui::Text("Single-image Sprite loaded from crew_1.png.");
        ImGui::Text("ResourceManager draws registered sprites each frame.");
        ImGui::End();
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;
        if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE))
            Engine::Engine::Quit();
    }
};

int main()
{
    Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_NEAREST);
    Engine::Engine engine;
    return engine.Run<StaticSpriteScene>("StaticSprite");
}
