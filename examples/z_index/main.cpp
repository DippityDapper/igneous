#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Vec2.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/resources/Sprite.hpp"

#include "imgui.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_render.h"

namespace
{
    void AddSheetFrame(Engine::Sprite& sprite, const std::string& sheet, int animRow)
    {
        Engine::Animation* anim = sprite.AddAnimation("frame", true);
        anim->SetFPS(1.0f);
        anim->AddFrame(sheet, 8, 4, animRow, 0);
    }
}

class ZIndexScene : public Engine::Scene
{
  public:
    Engine::Vec2<float> backPos{250, 200};
    Engine::Vec2<float> midPos{320, 180};
    Engine::Vec2<float> frontPos{390, 160};
    std::unique_ptr<Engine::Sprite> back;
    std::unique_ptr<Engine::Sprite> mid;
    std::unique_ptr<Engine::Sprite> front;

    void OnCreated() override
    {
        back = std::make_unique<Engine::Sprite>(backPos);
        front = std::make_unique<Engine::Sprite>(frontPos);
        mid = std::make_unique<Engine::Sprite>(midPos, "assets/sprites/crew_1.png");

        for (auto* sprite: {back.get(), mid.get(), front.get()})
        {
            sprite->centered = true;
            sprite->scaleX = sprite->scaleY = 3.0f;
        }

        AddSheetFrame(*back, "assets/sprite-sheets/colonist_green_spritesheet.png", 0);
        AddSheetFrame(*front, "assets/sprite-sheets/colonist_pink_spritesheet.png", 0);

        back->SetZIndex(0);
        mid->SetZIndex(1);
        front->SetZIndex(2);
    }

    void Render() override
    {
        ImGui::Begin("Z-index");
        ImGui::Text("Green colonist (back), blue square (mid), pink colonist (front).");
        ImGui::Text("ResourceManager sorts sprites by z-index when rendering.");
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
    return engine.Run<ZIndexScene>("ZIndex");
}
