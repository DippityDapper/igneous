#include "igneous/engine/Camera.hpp"
#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Time.hpp"
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

    void AddIdleDown(Engine::Sprite& sprite)
    {
        Engine::Animation* anim = sprite.AddAnimation("idle_down", true);
        anim->SetFPS(6.0f);
        for (int frame = 0; frame < 2; ++frame)
            anim->AddFrame(kSheet, 8, 4, 0, frame);
    }
}

class CameraScene : public Engine::Scene
{
  public:
    Engine::Camera camera{480, 270, 1.5f};
    Engine::Vec2<float> pos{480, 270};
    std::unique_ptr<Engine::Sprite> sprite;

    void OnCreated() override
    {
        camera.limitBounds = true;
        camera.limitLeft = 0;
        camera.limitTop = 0;
        camera.limitRight = 960;
        camera.limitBottom = 540;
        camera.minZoom = 0.5f;
        camera.maxZoom = 3.0f;

        sprite = std::make_unique<Engine::Sprite>(pos);
        sprite->centered = true;
        sprite->scaleX = sprite->scaleY = 3.0f;
        AddIdleDown(*sprite);
    }

    void Update(double delta) override
    {
        (void) delta;
        const float pan = 200.0f * static_cast<float>(Engine::Time::deltaTime);
        if (Engine::Input::IsKeyDown(SDLK_LEFT))
            camera.position.x -= pan;
        if (Engine::Input::IsKeyDown(SDLK_RIGHT))
            camera.position.x += pan;
        if (Engine::Input::IsKeyDown(SDLK_UP))
            camera.position.y -= pan;
        if (Engine::Input::IsKeyDown(SDLK_DOWN))
            camera.position.y += pan;

        auto wheel = Engine::Input::GetMouseWheelVelocity();
        if (wheel.y != 0.0f)
            camera.zoom = camera.ClampToBounds(camera.zoom + wheel.y * 0.1f);
    }

    void Render() override
    {
        ImGui::Begin("Camera");
        ImGui::Text("Arrows pan the camera. Mouse wheel zooms.");
        ImGui::Text("Position (%.0f, %.0f)  Zoom %.2f", camera.position.x, camera.position.y, camera.zoom);
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
    return engine.Run<CameraScene>("Camera");
}
