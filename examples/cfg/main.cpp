#include "igneous/engine/CFGParser.hpp"
#include "igneous/engine/Engine.hpp"
#include "igneous/input/Input.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

class CfgScene : public Engine::Scene
{
  public:
    int volume = 70;
    bool fullscreen = false;

    void OnCreated() override
    {
        Engine::CFGParser::LoadConfig("assets/settings.cfg", "game");
        try
        {
            volume = Engine::CFGParser::GetInt("game", "volume");
            fullscreen = Engine::CFGParser::GetBool("game", "fullscreen");
        }
        catch (...)
        {
            Engine::CFGParser::WriteInt("game", "volume", volume);
            Engine::CFGParser::WriteBool("game", "fullscreen", fullscreen);
            Engine::CFGParser::SaveConfig("assets/settings.cfg", "game");
        }
    }

    void Render() override
    {
        ImGui::Begin("CFGParser");
        if (ImGui::SliderInt("Volume", &volume, 0, 100))
        {
            Engine::CFGParser::WriteInt("game", "volume", volume);
            Engine::CFGParser::SaveConfig("assets/settings.cfg", "game");
        }
        if (ImGui::Checkbox("Fullscreen", &fullscreen))
        {
            Engine::CFGParser::WriteBool("game", "fullscreen", fullscreen);
            Engine::CFGParser::SaveConfig("assets/settings.cfg", "game");
        }
        ImGui::Text("Values persist to assets/settings.cfg");
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
    return engine.Run<CfgScene>("Cfg");
}
