#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Time.hpp"
#include "igneous/input/Input.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

class HelloScene : public Engine::Scene
{
  public:
    void Render() override
    {
        ImGui::Begin("Hello");
        ImGui::Text("Igneous engine loop is running.");
        ImGui::Text("Delta: %.4f s", Engine::Time::deltaTime);
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
    return engine.Run<HelloScene>("Hello");
}
