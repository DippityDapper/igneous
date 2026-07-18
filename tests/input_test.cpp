#include <catch2/catch_test_macros.hpp>

#include "SdlFixture.hpp"
#include "igneous/input/GamepadAxis.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/input/InputAction.hpp"
#include "igneous/input/InputEvent.hpp"
#include "igneous/input/InputLayer.hpp"
#include "igneous/input/InputMap.hpp"
#include "igneous/input/InputMapLoader.hpp"
#include "igneous/input/InputMapQuery.hpp"

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

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
