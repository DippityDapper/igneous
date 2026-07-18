#include "igneous/engine/Engine.hpp"
#include "igneous/input/GamepadAxis.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/input/InputMapLoader.hpp"

#include "imgui.h"
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

#include <algorithm>
#include <memory>
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

    constexpr const char* kMoveRight = "move_right";
    constexpr const char* kMoveLeft = "move_left";
    constexpr const char* kMoveUp = "move_up";
    constexpr const char* kMoveDown = "move_down";
    constexpr const char* kFire = "fire";
    constexpr const char* kJump = "jump";
    constexpr const char* kQuit = "quit";

    std::shared_ptr<Engine::InputMap> BuildFlexibleCodeMap()
    {
        auto map = std::make_shared<Engine::InputMap>("code", "Flexible Code Map");

        map->AddKeyBinding(kMoveRight, SDLK_D);
        map->AddKeyBinding(kMoveRight, SDLK_RIGHT);
        map->AddGamepadAxisBinding(kMoveRight, Engine::GamepadAxis::LeftXRight);

        map->AddKeyBinding(kMoveLeft, SDLK_A);
        map->AddKeyBinding(kMoveLeft, SDLK_LEFT);
        map->AddGamepadAxisBinding(kMoveLeft, Engine::GamepadAxis::LeftXLeft);

        map->AddKeyBinding(kMoveUp, SDLK_W);
        map->AddKeyBinding(kMoveUp, SDLK_UP);
        map->AddGamepadAxisBinding(kMoveUp, Engine::GamepadAxis::LeftYUp);

        map->AddKeyBinding(kMoveDown, SDLK_S);
        map->AddKeyBinding(kMoveDown, SDLK_DOWN);
        map->AddGamepadAxisBinding(kMoveDown, Engine::GamepadAxis::LeftYDown);

        map->AddKeyBinding(kFire, SDLK_F);
        map->AddMouseBinding(kFire, SDL_BUTTON_LEFT);
        map->AddGamepadAxisBinding(kFire, Engine::GamepadAxis::TriggerRight);

        map->AddKeyBinding(kJump, SDLK_SPACE);
        map->AddGamepadButtonBinding(kJump, SDL_GAMEPAD_BUTTON_SOUTH);

        map->AddKeyBinding(kQuit, SDLK_ESCAPE);
        return map;
    }
}

class InputActionsScene : public Engine::Scene
{
  public:
    bool useJsonMap = true;
    int fireCount = 0;
    FlashTimer fireFlash{};
    FlashTimer jumpFlash{};
    std::string lastEdgeEvent = "none";

    void OnCreated() override
    {
        Engine::Input::SetInputMap(Engine::InputMapLoader::LoadFromFile("assets/flexible_input_map.json"));
    }

    void Render() override
    {
        float moveX = Engine::Input::GetAxis(kMoveRight, kMoveLeft);
        float moveY = Engine::Input::GetAxis(kMoveDown, kMoveUp);

        ImGui::Begin("Input Actions");
        ImGui::Text("Source: %s", useJsonMap ? "assets/flexible_input_map.json" : "code-built map");
        ImGui::Separator();

        ImGui::Text("Movement axis: (%.2f, %.2f)", moveX, moveY);
        ImGui::Text("Fire value: %.2f (F, mouse left, or RT)", Engine::Input::GetAction(kFire));
        ImGui::Text("Jump held: %s", Engine::Input::IsActionPressed(kJump) ? "yes" : "no");
        ImGui::Text("Fire triggers this session: %d", fireCount);

        if (fireFlash.IsActive())
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.5f, 1.0f), "Fire just pressed");
        if (jumpFlash.IsActive())
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.5f, 1.0f), "Jump just pressed");

        ImGui::Separator();
        ImGui::Text("Last edge: %s", lastEdgeEvent.c_str());
        ImGui::Separator();

        ImGui::TextUnformatted("Each action mixes device bindings:");
        ImGui::BulletText("move_*: keyboard + arrow keys + gamepad stick halves");
        ImGui::BulletText("fire: keyboard F + mouse left + right trigger");
        ImGui::BulletText("jump: Space + gamepad A");

        if (ImGui::Button("Toggle JSON / code map"))
        {
            useJsonMap = !useJsonMap;
            Engine::Input::SetInputMap(
                useJsonMap
                    ? Engine::InputMapLoader::LoadFromFile("assets/flexible_input_map.json")
                    : BuildFlexibleCodeMap());
        }

        ImGui::End();
    }

    void Update(double delta) override
    {
        fireFlash.Update(delta);
        jumpFlash.Update(delta);
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;

        if (Engine::Input::IsActionJustPressed(kFire))
        {
            ++fireCount;
            fireFlash.Trigger();
            lastEdgeEvent = "fire just pressed";
        }

        if (Engine::Input::IsActionJustPressed(kJump))
        {
            jumpFlash.Trigger();
            lastEdgeEvent = "jump just pressed";
        }

        if (Engine::Input::IsActionJustReleased(kJump))
            lastEdgeEvent = "jump just released";

        if (Engine::Input::IsActionJustPressed(kQuit))
            Engine::Engine::Quit();
    }
};

int main()
{
    Engine::Engine engine;
    return engine.Run<InputActionsScene>("Input Actions");
}
