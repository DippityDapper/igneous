#include "igneous/engine/Engine.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/networking/LocalIdentity.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

#include <memory>

class IdentityScene : public Engine::Scene
{
  public:
    std::unique_ptr<Engine::LocalIdentity> identity;

    void OnCreated() override
    {
        identity = std::make_unique<Engine::LocalIdentity>("assets/user.txt");
        identity->GenerateLocalId();
        identity->GenerateLocalUsername();
    }

    void Render() override
    {
        ImGui::Begin("LocalIdentity");
        ImGui::Text("User file: assets/user.txt");
        ImGui::Text("ID: %llu", static_cast<unsigned long long>(identity->GetLocalId()));
        ImGui::Text("Username: %s", identity->GetLocalUsername().c_str());

        if (ImGui::Button("Generate auth token"))
        {
            auto token = identity->GetAuthToken(999);
            ImGui::Text("Token bytes: %zu", token.size());
        }
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
    return engine.Run<IdentityScene>("Identity");
}
