#include "igneous/engine/Engine.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/resources/AudioStream.hpp"
#include "igneous/resources/ResourceManager.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

#include <memory>
#include <vector>

class AudioScene : public Engine::Scene
{
  public:
    std::vector<std::shared_ptr<Engine::AudioStream>> buttons;
    std::vector<std::shared_ptr<Engine::AudioStream>> steps;
    int buttonIndex = 0;
    int stepIndex = 0;
    float footstepTimer = 0.0f;

    void OnCreated() override
    {
        for (int i = 1; i <= 5; ++i)
            buttons.push_back(Engine::ResourceManager::LoadSound("assets/sounds/button/button" + std::to_string(i) + ".wav", 0));

        for (int i = 1; i <= 4; ++i)
        {
            steps.push_back(Engine::ResourceManager::LoadSound("assets/sounds/footsteps/concrete" + std::to_string(i) + ".wav", 0));
            steps.push_back(Engine::ResourceManager::LoadSound("assets/sounds/footsteps/dirt" + std::to_string(i) + ".wav", 0));
        }
    }

    void Update(double delta) override
    {
        footstepTimer -= static_cast<float>(delta);
        if (footstepTimer <= 0.0f && (Engine::Input::IsKeyDown(SDLK_W) || Engine::Input::IsKeyDown(SDLK_A) ||
                                      Engine::Input::IsKeyDown(SDLK_S) || Engine::Input::IsKeyDown(SDLK_D)))
        {
            if (!steps.empty())
            {
                steps[stepIndex % steps.size()]->Play(0.8f);
                stepIndex++;
            }
            footstepTimer = 0.35f;
        }
    }

    void Render() override
    {
        ImGui::Begin("Audio");
        ImGui::Text("Click buttons to play UI sounds.");
        if (ImGui::Button("Play button sound"))
        {
            if (!buttons.empty())
            {
                buttons[buttonIndex % buttons.size()]->Play(1.0f);
                buttonIndex++;
            }
        }
        ImGui::Text("Hold WASD to loop footstep samples.");
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
    return engine.Run<AudioScene>("Audio");
}
