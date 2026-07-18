#include <catch2/catch_test_macros.hpp>

#include "TestReset.hpp"
#include "SdlFixture.hpp"
#include "igneous/input/GamepadAxis.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/input/InputBinding.hpp"
#include "igneous/input/InputAction.hpp"
#include "igneous/input/InputEvent.hpp"
#include "igneous/input/InputLayer.hpp"
#include "igneous/input/InputMap.hpp"
#include "igneous/input/InputMapLoader.hpp"
#include "igneous/input/InputMapQuery.hpp"

#include "igneous/rendering/Renderer.hpp"
#include "igneous/rendering/Window.hpp"

#include "imgui.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

struct InputPollFixture
{
    InputPollFixture()
    {
        Engine::TestReset::All();
        REQUIRE(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD));
        Engine::Window::Init(640, 360);
        Engine::Renderer::Init();
        ImGui::GetIO().WantCaptureKeyboard = false;
        ImGui::GetIO().WantCaptureMouse = false;
        Engine::Input::Init();
    }

    ~InputPollFixture()
    {
        Engine::Input::ResetForTests();
        Engine::Renderer::Clean();
        Engine::Window::Clean();
        SDL_Quit();
        Engine::TestReset::All();
    }
};

namespace
{
    void PumpInputEvent(SDL_Event& event)
    {
        ImGui::GetIO().WantCaptureKeyboard = false;
        ImGui::GetIO().WantCaptureMouse = false;
        Engine::Input::HandleEvent(event);
    }

    void PumpKeyEvent(SDL_EventType type, SDL_Keycode key)
    {
        SDL_Event event{};
        event.type = type;
        event.key.key = key;
        event.key.down = (type == SDL_EVENT_KEY_DOWN);
        PumpInputEvent(event);
    }

    void PumpMouseButtonEvent(SDL_EventType type, SDL_MouseButtonFlags button)
    {
        SDL_Event event{};
        event.type = type;
        event.button.button = button;
        event.button.down = (type == SDL_EVENT_MOUSE_BUTTON_DOWN);
        PumpInputEvent(event);
    }

    void PumpGamepadButtonEvent(SDL_JoystickID id, SDL_GamepadButton button, bool down)
    {
        SDL_Event added{};
        added.type = SDL_EVENT_GAMEPAD_ADDED;
        added.gdevice.which = id;
        PumpInputEvent(added);

        SDL_Event event{};
        event.type = down ? SDL_EVENT_GAMEPAD_BUTTON_DOWN : SDL_EVENT_GAMEPAD_BUTTON_UP;
        event.gbutton.which = id;
        event.gbutton.button = static_cast<Uint8>(button);
        event.gbutton.down = down;
        PumpInputEvent(event);
    }

    void PumpGamepadAxisEvent(SDL_JoystickID id, SDL_GamepadAxis axis, Sint16 value)
    {
        SDL_Event added{};
        added.type = SDL_EVENT_GAMEPAD_ADDED;
        added.gdevice.which = id;
        PumpInputEvent(added);

        SDL_Event event{};
        event.type = SDL_EVENT_GAMEPAD_AXIS_MOTION;
        event.gaxis.which = id;
        event.gaxis.axis = static_cast<Uint8>(axis);
        event.gaxis.value = value;
        PumpInputEvent(event);
    }
}

TEST_CASE("GamepadAxis string conversion", "[input][gamepadaxis]")
{
    Engine::GamepadAxis axis{};
    REQUIRE(Engine::GamepadAxisFromString("left_x_right", axis));
    REQUIRE(axis == Engine::GamepadAxis::LeftXRight);
    REQUIRE(std::string(Engine::GamepadAxisToString(axis)) == "left_x_right");
    REQUIRE_FALSE(Engine::GamepadAxisFromString("invalid_axis", axis));
}

TEST_CASE("InputLayer name and priority", "[input][inputlayer]")
{
    Engine::InputLayer layer("gameplay", 2);
    REQUIRE(layer.GetName() == "gameplay");
    REQUIRE(layer.GetPriority() == 2);
    REQUIRE(layer.Is("gameplay"));
    REQUIRE_FALSE(layer.Is("menu"));
}

TEST_CASE("InputEvent default state", "[input][inputevent]")
{
    Engine::InputEvent event{};
    REQUIRE_FALSE(event.pressed);
    REQUIRE_FALSE(event.pressedLastFrame);
    REQUIRE_FALSE(event.handled);
}

TEST_CASE("InputMap actions and bindings", "[input][inputmap]")
{
    auto map = std::make_shared<Engine::InputMap>("test", "Test");
    map->AddKeyBinding("jump", SDLK_SPACE);
    map->AddMouseBinding("fire", SDL_BUTTON_LEFT);
    map->AddGamepadAxisBinding("move_right", Engine::GamepadAxis::LeftXRight);

    REQUIRE(map->FindAction("jump") != nullptr);
    REQUIRE(map->FindAction("missing") == nullptr);
    REQUIRE(map->GetActions().size() == 3);

    map->RemoveAction("fire");
    REQUIRE(map->FindAction("fire") == nullptr);
}

TEST_CASE("InputAction with no bindings returns zero", "[input][inputaction]")
{
    Engine::InputAction action("empty");
    REQUIRE(action.GetValue() == 0.0f);
    REQUIRE_FALSE(action.IsPressed());
    REQUIRE_FALSE(action.IsJustPressed());
}

TEST_CASE("InputAction aggregates first active binding value", "[input][inputaction]")
{
    InputPollFixture fixture;
    (void) fixture;

    Engine::InputAction jump("jump");
    jump.AddBinding(Engine::MakeKeyBinding(SDLK_SPACE));
    jump.AddBinding(Engine::MakeMouseButtonBinding(SDL_BUTTON_LEFT));

    Engine::Input::ResetEvents();
    PumpKeyEvent(SDL_EVENT_KEY_DOWN, SDLK_SPACE);
    REQUIRE(jump.GetValue() == 1.0f);

    Engine::Input::ResetEvents();
    PumpKeyEvent(SDL_EVENT_KEY_UP, SDLK_SPACE);
    PumpMouseButtonEvent(SDL_EVENT_MOUSE_BUTTON_DOWN, SDL_BUTTON_LEFT);
    REQUIRE(jump.GetValue() == 1.0f);
}

TEST_CASE("InputMapQuery axis composition", "[input][inputmapquery]")
{
    REQUIRE(Engine::InputMapQuery::GetAxisFromValues(1.0f, 0.0f) == 1.0f);
    REQUIRE(Engine::InputMapQuery::GetAxisFromValues(0.0f, 1.0f) == -1.0f);
    REQUIRE(Engine::InputMapQuery::GetAxisFromValues(0.5f, 0.5f) == 0.0f);
}

TEST_CASE("InputMapLoader round trip json", "[input][inputmaploader]")
{
    const char* json = R"({
      "id": "test",
      "name": "Test",
      "actions": [
        {
          "name": "jump",
          "bindings": [
            { "type": "key", "key": "Space" },
            { "type": "gamepad_button", "button": "a" }
          ]
        }
      ]
    })";

    std::shared_ptr<Engine::InputMap> map = Engine::InputMapLoader::LoadFromString(json);
    REQUIRE(map->GetId() == "test");
    REQUIRE(map->GetName() == "Test");
    REQUIRE(map->FindAction("jump") != nullptr);
    REQUIRE(map->FindAction("jump")->GetBindings().size() == 2);

    std::string saved = Engine::InputMapLoader::SaveToString(*map);
    REQUIRE(saved.find("\"jump\"") != std::string::npos);
}

TEST_CASE("Input layer priority sort", "[input][input]")
{
    SdlFixture sdl;
    (void) sdl;

    Engine::Input::RemoveInputLayer("gameplay");
    Engine::Input::RemoveInputLayer("menu");
    Engine::Input::AddInputLayer("gameplay", 0);
    Engine::Input::AddInputLayer("menu", 2);

    auto layers = Engine::Input::GetInputLayers();
    REQUIRE(layers.size() >= 2);
    REQUIRE(layers.front()->GetPriority() >= layers.back()->GetPriority());

    Engine::Input::RemoveInputLayer("gameplay");
    Engine::Input::RemoveInputLayer("menu");
}

TEST_CASE("InputBinding polls key mouse and gamepad state", "[input][inputbinding]")
{
    InputPollFixture fixture;
    (void) fixture;

    Engine::KeyBinding keyBinding(SDLK_A);
    Engine::MouseButtonBinding mouseBinding(SDL_BUTTON_RIGHT);
    Engine::GamepadButtonBinding gamepadBinding(SDL_GAMEPAD_BUTTON_SOUTH);
    Engine::GamepadAxisBinding axisBinding(Engine::GamepadAxis::LeftXRight);

    Engine::Input::ResetEvents();
    REQUIRE(keyBinding.GetValue() == 0.0f);
    PumpKeyEvent(SDL_EVENT_KEY_DOWN, SDLK_A);
    REQUIRE(keyBinding.GetValue() == 1.0f);

    Engine::Input::ResetEvents();
    REQUIRE(mouseBinding.GetValue() == 0.0f);
    PumpMouseButtonEvent(SDL_EVENT_MOUSE_BUTTON_DOWN, SDL_BUTTON_RIGHT);
    REQUIRE(mouseBinding.GetValue() == 1.0f);

    constexpr SDL_JoystickID gamepadId = 42;
    PumpGamepadButtonEvent(gamepadId, SDL_GAMEPAD_BUTTON_SOUTH, true);
    if (!Engine::Input::GetConnectedGamepads().empty())
    {
        REQUIRE(gamepadBinding.GetValue() == 1.0f);

        Engine::Input::ResetEvents();
        PumpGamepadAxisEvent(gamepadId, SDL_GAMEPAD_AXIS_LEFTX, 28000);
        REQUIRE(axisBinding.GetValue() > 0.0f);
    }
}
