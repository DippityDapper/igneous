#include "igneous/engine/Engine.hpp"
#include "igneous/input/Input.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

#include <algorithm>
#include <string>

namespace
{
    struct FlashTimer
    {
        double remaining = 0.0;

        void Trigger(double duration = 0.5)
        {
            remaining = duration;
        }

        void Update(double delta)
        {
            remaining = std::max(0.0, remaining - delta);
        }

        bool IsActive() const
        {
            return remaining > 0.0;
        }
    };

    struct KeySample
    {
        SDL_Keycode key;
        const char* label;
        FlashTimer flash{};
    };

    KeySample kWatchedKeys[] = {
        {SDLK_W, "W", {}},
        {SDLK_A, "A", {}},
        {SDLK_S, "S", {}},
        {SDLK_D, "D", {}},
        {SDLK_SPACE, "Space", {}},
        {SDLK_LSHIFT, "Shift", {}},
        {SDLK_ESCAPE, "Escape", {}},
    };

    std::string lastEdgeEvent = "none";
}

class KeyboardInputScene : public Engine::Scene
{
  public:
    void Render() override
    {
        ImGui::Begin("Keyboard Input");
        ImGui::Text("Raw keyboard queries via Input::IsKey*");
        ImGui::Text("Rows flash green for 0.5s after just-pressed.");
        ImGui::Separator();

        for (KeySample& sample: kWatchedKeys)
        {
            if (sample.flash.IsActive())
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.5f, 1.0f));

            ImGui::Text(
                "%s: down=%s  handled=%s%s",
                sample.label,
                Engine::Input::IsKeyDown(sample.key, false) ? "yes" : "no",
                Engine::Input::IsKeyHandled(sample.key) ? "yes" : "no",
                sample.flash.IsActive() ? "  [just pressed]" : "");

            if (sample.flash.IsActive())
                ImGui::PopStyleColor();
        }

        ImGui::Separator();
        ImGui::Text("Last edge: %s", lastEdgeEvent.c_str());
        ImGui::Separator();
        ImGui::Text("Press Escape to quit.");
        ImGui::End();
    }

    void Update(double delta) override
    {
        for (KeySample& sample: kWatchedKeys)
            sample.flash.Update(delta);
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;

        for (KeySample& sample: kWatchedKeys)
        {
            if (Engine::Input::IsKeyJustPressed(sample.key))
            {
                sample.flash.Trigger();
                lastEdgeEvent = std::string(sample.label) + " just pressed";
            }
            if (Engine::Input::IsKeyJustReleased(sample.key))
                lastEdgeEvent = std::string(sample.label) + " just released";
        }

        if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE))
            Engine::Engine::Quit();
    }
};

int main()
{
    Engine::Engine engine;
    return engine.Run<KeyboardInputScene>("Keyboard Input");
}
