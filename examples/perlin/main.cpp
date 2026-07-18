#include "igneous/engine/Engine.hpp"
#include "igneous/engine/PerlinNoise.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/rendering/Renderer.hpp"
#include "igneous/resources/ResourceManager.hpp"

#include "imgui.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_render.h"

#include <memory>

class PerlinScene : public Engine::Scene
{
  public:
    Engine::Perlin noise;
    std::shared_ptr<SDL_Texture> preview;
    bool dirty = true;

    void Regenerate()
    {
        noise.width = 256;
        noise.height = 256;
        noise.SavePng("assets/noise_preview.png");
        preview = Engine::ResourceManager::LoadTexture("assets/noise_preview.png");
        dirty = false;
    }

    void OnCreated() override
    {
        noise.SetSeed(42);
        Regenerate();
    }

    void Render() override
    {
        if (dirty)
            Regenerate();

        if (preview)
            Engine::Renderer::BufferAdd({120, 120}, preview.get(), false);

        ImGui::Begin("Perlin noise");
        ImGui::SliderInt("Octaves", &noise.octaves, 1, 8);
        ImGui::SliderFloat("Scale", &noise.scale, 0.001f, 0.02f);
        if (ImGui::Button("Regenerate"))
            dirty = true;
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
    return engine.Run<PerlinScene>("Perlin");
}
