#include "igneous/engine/Engine.hpp"
#include "igneous/input/Input.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

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

    struct MouseButtonSample
    {
        SDL_MouseButtonFlags button;
        const char* label;
        FlashTimer pressFlash{};
        FlashTimer releaseFlash{};
    };

    MouseButtonSample kButtons[] = {
        {SDL_BUTTON_LEFT, "Left", {}, {}},
        {SDL_BUTTON_RIGHT, "Right", {}, {}},
        {SDL_BUTTON_MIDDLE, "Middle", {}, {}},
    };

    std::string lastEdgeEvent = "none";
}

class MouseInputScene : public Engine::Scene
{
  public:
    Engine::Vec2<float> marker{320, 180};

    void Render() override
    {
        auto pos = Engine::Input::GetMouseScreenPosition();
        auto vel = Engine::Input::GetMouseVelocity();
        auto wheel = Engine::Input::GetMouseWheelVelocity();

        ImGui::Begin("Mouse Input");
        ImGui::Text("Screen position: (%.0f, %.0f)", pos.x, pos.y);
        ImGui::Text("Velocity: (%.0f, %.0f)", vel.x, vel.y);
        ImGui::Text("Wheel delta: (%.0f, %.0f)", wheel.x, wheel.y);
        ImGui::Separator();

        ImGui::TextUnformatted("Buttons:");
        for (MouseButtonSample& sample: kButtons)
        {
            if (sample.pressFlash.IsActive() || sample.releaseFlash.IsActive())
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.5f, 1.0f));

            ImGui::Text(
                "%s: down=%s%s%s",
                sample.label,
                Engine::Input::IsMouseButtonDown(sample.button, false) ? "yes" : "no",
                sample.pressFlash.IsActive() ? "  [just pressed]" : "",
                sample.releaseFlash.IsActive() ? "  [just released]" : "");

            if (sample.pressFlash.IsActive() || sample.releaseFlash.IsActive())
                ImGui::PopStyleColor();
        }

        ImGui::Separator();
        ImGui::Text("Last edge: %s", lastEdgeEvent.c_str());
        ImGui::Separator();
        ImGui::Text("Marker (hold left button to drag): (%.0f, %.0f)", marker.x, marker.y);
        ImGui::Text("Scroll wheel moves marker vertically.");
        ImGui::Text("Press Escape to quit.");
        ImGui::End();
    }

    void Update(double delta) override
    {
        auto wheel = Engine::Input::GetMouseWheelVelocity();
        marker.y -= wheel.y * 16.0f;

        if (Engine::Input::IsMouseButtonDown(SDL_BUTTON_LEFT, false))
            marker = Engine::Input::GetMouseScreenPosition();

        for (MouseButtonSample& sample: kButtons)
        {
            sample.pressFlash.Update(delta);
            sample.releaseFlash.Update(delta);
        }
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;

        auto wheel = Engine::Input::GetMouseWheelVelocity();
        if (wheel.y != 0.0f || wheel.x != 0.0f)
        {
            lastEdgeEvent = "Wheel ("
                + std::to_string(static_cast<int>(wheel.x))
                + ", "
                + std::to_string(static_cast<int>(wheel.y))
                + ")";
        }

        for (MouseButtonSample& sample: kButtons)
        {
            if (Engine::Input::IsButtonJustPressed(sample.button))
            {
                sample.pressFlash.Trigger();
                lastEdgeEvent = std::string(sample.label) + " just pressed";
            }
            if (Engine::Input::IsButtonJustReleased(sample.button))
            {
                sample.releaseFlash.Trigger();
                lastEdgeEvent = std::string(sample.label) + " just released";
            }
        }

        if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE))
            Engine::Engine::Quit();
    }
};

int main()
{
    Engine::Engine engine;
    return engine.Run<MouseInputScene>("Mouse Input");
}
