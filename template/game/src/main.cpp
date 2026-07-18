#include "igneous/engine/Engine.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/scenes/Scene.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

class MainScene : public Engine::Scene
{
  public:
    void Render() override
    {
        ImGui::Begin("My Game");
        ImGui::Text("Igneous is linked and running.");
        if (ImGui::Button("Quit"))
            Engine::Engine::Quit();
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
    Engine::Engine engine;
    return engine.Run<MainScene>("Main");
}
