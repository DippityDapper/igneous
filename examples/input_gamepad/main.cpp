#include "igneous/engine/Engine.hpp"
#include "igneous/input/GamepadAxis.hpp"
#include "igneous/input/Input.hpp"

#include "imgui.h"
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keycode.h>

#include <algorithm>
#include <string>
#include <unordered_map>

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
}

class GamepadInputScene : public Engine::Scene
{
  public:
    SDL_JoystickID activePad = 0;
    float rumbleLow = 0.5f;
    float rumbleHigh = 0.5f;
    float triggerRumble = 0.5f;
    uint32_t rumbleDurationMs = 300;
    int rumbleDurationUi = 300;
    std::string lastEdgeEvent = "none";
    std::unordered_map<int, FlashTimer> buttonFlashes{};

    void OnCreated() override
    {
        RefreshActivePad();
    }

    void RefreshActivePad()
    {
        auto pads = Engine::Input::GetConnectedGamepads();
        activePad = pads.empty() ? 0 : pads.front();
    }

    FlashTimer& GetButtonFlash(SDL_GamepadButton button)
    {
        return buttonFlashes[static_cast<int>(button)];
    }

    void PollButtonEdges(SDL_JoystickID pad)
    {
        for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; ++i)
        {
            auto button = static_cast<SDL_GamepadButton>(i);
            const char* name = SDL_GetGamepadStringForButton(button);
            if (!name)
                continue;

            if (Engine::Input::IsGamepadButtonJustPressed(pad, button))
            {
                GetButtonFlash(button).Trigger();
                lastEdgeEvent = std::string(name) + " just pressed";
            }
            if (Engine::Input::IsGamepadButtonJustReleased(pad, button))
                lastEdgeEvent = std::string(name) + " just released";
        }
    }

    void RenderGamepadPanel(SDL_JoystickID pad)
    {
        ImGui::TextUnformatted("Buttons (green flash on just-pressed):");
        for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; ++i)
        {
            auto button = static_cast<SDL_GamepadButton>(i);
            const char* name = SDL_GetGamepadStringForButton(button);
            if (!name)
                continue;

            bool down = Engine::Input::IsGamepadButtonDown(pad, button, false);
            bool flash = GetButtonFlash(button).IsActive();
            if (!down && !flash)
                continue;

            if (flash)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.5f, 1.0f));

            ImGui::Text("%s: down=%s%s", name, down ? "yes" : "no", flash ? "  [just pressed]" : "");

            if (flash)
                ImGui::PopStyleColor();
        }

        ImGui::Separator();
        ImGui::Text("Left stick (raw): (%.2f, %.2f)",
            Engine::Input::GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTX),
            Engine::Input::GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTY));
        ImGui::Text("Right stick (raw): (%.2f, %.2f)",
            Engine::Input::GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTX),
            Engine::Input::GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTY));
        ImGui::Text("Triggers: L=%.2f R=%.2f",
            Engine::Input::GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER),
            Engine::Input::GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER));

        ImGui::Separator();
        ImGui::Text("Semantic axes (directional halves, 0.3 deadzone):");
        ImGui::Text("Left X:  right=%.2f left=%.2f",
            Engine::Input::GetSemanticGamepadAxis(pad, Engine::GamepadAxis::LeftXRight, 0.3f, false),
            Engine::Input::GetSemanticGamepadAxis(pad, Engine::GamepadAxis::LeftXLeft, 0.3f, false));
        ImGui::Text("Left Y:  down=%.2f up=%.2f",
            Engine::Input::GetSemanticGamepadAxis(pad, Engine::GamepadAxis::LeftYDown, 0.3f, false),
            Engine::Input::GetSemanticGamepadAxis(pad, Engine::GamepadAxis::LeftYUp, 0.3f, false));

        auto semanticStick = Engine::Input::GetSemanticGamepadStick(
            pad,
            Engine::GamepadAxis::LeftXRight,
            Engine::GamepadAxis::LeftYDown,
            0.3f,
            false);
        ImGui::Text("Semantic left stick vector: (%.2f, %.2f)", semanticStick.x, semanticStick.y);

        ImGui::Separator();
        ImGui::SliderFloat("Motor low", &rumbleLow, 0.0f, 1.0f);
        ImGui::SliderFloat("Motor high", &rumbleHigh, 0.0f, 1.0f);
        ImGui::SliderInt("Duration ms", &rumbleDurationUi, 50, 2000);
        rumbleDurationMs = static_cast<uint32_t>(rumbleDurationUi);

        if (ImGui::Button("Rumble motors"))
            Engine::Input::RumbleGamepad(pad, rumbleLow, rumbleHigh, rumbleDurationMs);

        ImGui::SliderFloat("Trigger rumble", &triggerRumble, 0.0f, 1.0f);
        if (ImGui::Button("Rumble triggers"))
            Engine::Input::RumbleGamepadTriggers(pad, triggerRumble, triggerRumble, rumbleDurationMs);
    }

    void Render() override
    {
        auto pads = Engine::Input::GetConnectedGamepads();

        ImGui::Begin("Gamepad Input");
        ImGui::Text("Connected gamepads: %zu", pads.size());

        if (pads.empty())
        {
            ImGui::TextUnformatted("Plug in a controller and press Refresh.");
            if (ImGui::Button("Refresh"))
                RefreshActivePad();
        }
        else
        {
            std::string activeLabel = std::to_string(activePad);
            if (ImGui::BeginCombo("Active pad", activeLabel.c_str()))
            {
                for (SDL_JoystickID pad: pads)
                {
                    std::string padLabel = std::to_string(pad);
                    if (ImGui::Selectable(padLabel.c_str(), pad == activePad))
                        activePad = pad;
                }
                ImGui::EndCombo();
            }

            RenderGamepadPanel(activePad);
        }

        ImGui::Separator();
        ImGui::Text("Last edge: %s", lastEdgeEvent.c_str());
        ImGui::Separator();
        ImGui::Text("Press Escape to quit.");
        ImGui::End();
    }

    void Update(double delta) override
    {
        for (auto& [button, flash]: buttonFlashes)
            (void) button, flash.Update(delta);
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;

        auto pads = Engine::Input::GetConnectedGamepads();
        for (SDL_JoystickID pad: pads)
            PollButtonEdges(pad);

        if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE))
            Engine::Engine::Quit();
    }
};

int main()
{
    Engine::Engine engine;
    return engine.Run<GamepadInputScene>("Gamepad Input");
}
