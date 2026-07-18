#include "igneous/engine/Camera.hpp"
#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Vec2.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/rendering/Renderer.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/resources/Sprite.hpp"

#include "imgui.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_render.h"

class RendererScene : public Engine::Scene
{
  public:
    Engine::Camera camera{320, 180, 1.0f};
    Engine::Vec2<float> worldPos{320, 180};
    std::unique_ptr<Engine::Sprite> worldSprite;
    std::shared_ptr<SDL_Texture> hudTexture;

    void OnCreated() override
    {
        camera.position = {320, 180};
        worldSprite = std::make_unique<Engine::Sprite>(worldPos, "assets/sprites/crew_1.png");
        worldSprite->centered = true;
        hudTexture = Engine::ResourceManager::LoadTexture("assets/sprites/crew_1.png");
    }

    void Render() override
    {
        if (hudTexture)
            Engine::Renderer::BufferAdd({24, 24}, hudTexture.get(), false);

        ImGui::Begin("Renderer");
        ImGui::Text("World sprite via ResourceManager (camera space).");
        ImGui::Text("Top-left portrait via Renderer::BufferAdd screen space.");
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
    return engine.Run<RendererScene>("Renderer");
}
