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
    constexpr const char* kSheet = "assets/sprite-sheets/colonist_blue_spritesheet.png";
    constexpr const char* kAnimNames[] = {
            "idle_down", "idle_right", "idle_left", "idle_up",
            "walk_down", "walk_right", "walk_left", "walk_up"};

    void AddSheetAnimation(Engine::Sprite& sprite, const char* name, int animRow, int frameCount, bool current)
    {
        Engine::Animation* anim = sprite.AddAnimation(name, current);
        if (!anim)
            return;
        anim->SetFPS(8.0f);
        for (int frame = 0; frame < frameCount; ++frame)
            anim->AddFrame(kSheet, 8, 4, animRow, frame);
    }
}

class AnimationScene : public Engine::Scene
{
  public:
    Engine::Vec2<float> pos{320, 180};
    std::unique_ptr<Engine::Sprite> sprite;
    std::string currentAnimation = "idle";
    std::string direction = "down";

    void OnCreated() override
    {
        sprite = std::make_unique<Engine::Sprite>(pos);
        sprite->centered = true;
        sprite->scaleX = sprite->scaleY = 3.0f;

        for (int i = 0; i < 4; ++i)
            AddSheetAnimation(*sprite, kAnimNames[i], i, 2, i == 0);
        for (int i = 4; i < 8; ++i)
            AddSheetAnimation(*sprite, kAnimNames[i], i, 4, false);
    }

    void Update(double delta) override
    {
        (void) delta;
        if (!sprite)
            return;

        if (Engine::Input::IsKeyDown(SDLK_W))
            direction = "up";
        else if (Engine::Input::IsKeyDown(SDLK_S))
            direction = "down";
        else if (Engine::Input::IsKeyDown(SDLK_A))
            direction = "left";
        else if (Engine::Input::IsKeyDown(SDLK_D))
            direction = "right";

        if (Engine::Input::IsKeyDown(SDLK_LSHIFT))
            currentAnimation = "walk";
        else
            currentAnimation = "idle";

        std::string animation = currentAnimation + "_" + direction;
        sprite->SetCurrentAnimation(animation, true);
    }

    void Render() override
    {
        ImGui::Begin("Animation");
        ImGui::Text("8x4 spritesheet: rows = down/right/left/up, columns = frames.");
        ImGui::Text("WASD = direction, Shift = walk cycle.");
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
    return engine.Run<AnimationScene>("Animation");
}
