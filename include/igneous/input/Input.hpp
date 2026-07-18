// Doc: docs/classes/Input.md
#pragma once

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "igneous/engine/Vec2.hpp"
#include "igneous/input/GamepadAxis.hpp"
#include "igneous/input/InputEvent.hpp"
#include "igneous/input/InputLayer.hpp"
#include "SDL3/SDL_events.h"

namespace Engine
{

    class InputMap;
    class InputAction;

    class Input
    {
      public:

        static constexpr const char* GameplayLayer = "gameplay";
        static constexpr const char* UILayer = "ui";
        static constexpr const char* MenuLayer = "menu";

      private:

        struct SemanticAxisState
        {
            float value = 0.0f;
            bool handled = false;
        };

        static inline std::map<std::string, std::unique_ptr<InputLayer>> layers{};

        static inline std::unordered_map<SDL_Keycode, InputEvent> keyEvents{};

        static inline std::unordered_map<SDL_MouseButtonFlags, InputEvent> mouseEvents{};

        static inline std::unordered_map<SDL_JoystickID, SDL_Gamepad*> gamepads{};

        static inline std::unordered_map<SDL_JoystickID, std::unordered_map<SDL_GamepadButton, InputEvent>> gamepadButtonEvents{};

        static inline std::unordered_map<SDL_JoystickID, std::unordered_map<SDL_GamepadAxis, float>> gamepadAxisValues{};

        static inline std::unordered_map<SDL_JoystickID, std::unordered_map<GamepadAxis, SemanticAxisState>> gamepadSemanticAxisStates{};

        static inline std::shared_ptr<InputMap> inputMap{};

        static inline float mouseX = 0;

        static inline float mouseY = 0;

        static inline float mouseVelX = 0;

        static inline float mouseVelY = 0;

        static inline float mouseWheelVelX = 0;

        static inline float mouseWheelVelY = 0;

        static void UpdateSemanticGamepadAxis(SDL_JoystickID instanceId, SDL_GamepadAxis axis, float value);

      public:

        static bool Init();

        static bool InitGamepads();

        /// Clears static input state between unit tests.
        static void ResetForTests();

        /// Restores the default input layer after shutdown or test reset.
        static void RestoreBaseline();

        static void ResetEvents();

        static void HandleEvent(SDL_Event& event);

        static bool AddInputLayer(const std::string& layerName, int priority);

        static bool RemoveInputLayer(const std::string& layerName);

        static std::vector<InputLayer*> GetInputLayers();

        static void SetInputMap(std::shared_ptr<InputMap> map);

        static InputMap* GetInputMap();

        static InputAction* GetInputAction(const std::string& actionName);

        static float GetAction(const std::string& actionName);

        static bool IsActionPressed(const std::string& actionName);

        static bool IsActionJustPressed(const std::string& actionName);

        static bool IsActionJustReleased(const std::string& actionName);

        static float GetAxis(const std::string& positiveAction, const std::string& negativeAction);

        static void HandleKey(SDL_Keycode key);

        static void HandleButton(SDL_MouseButtonFlags mouseButton);

        static void HandleAllKeys(const std::vector<SDL_Keycode>* exceptions = nullptr);

        static void HandleAllButtons(const std::vector<SDL_MouseButtonFlags>* exceptions = nullptr);

        static void HandleAllGamepadButtons(const std::vector<SDL_GamepadButton>* exceptions = nullptr);

        static void HandleAllGamepadAxes(const std::vector<GamepadAxis>* exceptions = nullptr);

        static void HandleAllInputs(
            const std::vector<SDL_Keycode>* keyExceptions = nullptr,
            const std::vector<SDL_MouseButtonFlags>* mouseButtonExceptions = nullptr,
            const std::vector<SDL_GamepadButton>* gamepadButtonExceptions = nullptr,
            const std::vector<GamepadAxis>* gamepadAxisExceptions = nullptr);

        static bool IsKeyHandled(SDL_Keycode key);

        static bool IsButtonHandled(SDL_MouseButtonFlags mouseButton);

        static bool IsKeyDown(SDL_Keycode key, bool skipIfHandled = true);

        static bool IsKeyJustPressed(SDL_Keycode key, bool skipIfHandled = true);

        static bool IsKeyJustReleased(SDL_Keycode key);

        static bool IsMouseButtonDown(SDL_MouseButtonFlags mouseButton, bool skipIfHandled = true);

        static bool IsButtonJustPressed(SDL_MouseButtonFlags mouseButton, bool skipIfHandled = true);

        static bool IsButtonJustReleased(SDL_MouseButtonFlags mouseButton);

        /// Delegates to `Window::WasResized()`.
        static bool IsWindowResized();

        static Vec2<float> GetMouseScreenPosition();

        static Vec2<float> GetMouseVelocity();

        static Vec2<float> GetMouseWheelVelocity();

        static bool IsGamepadButtonDown(SDL_JoystickID instanceId, SDL_GamepadButton button, bool skipIfHandled = true);

        static bool IsGamepadButtonJustPressed(SDL_JoystickID instanceId, SDL_GamepadButton button, bool skipIfHandled = true);

        static bool IsGamepadButtonJustReleased(SDL_JoystickID instanceId, SDL_GamepadButton button);

        static float GetGamepadAxis(SDL_JoystickID instanceId, SDL_GamepadAxis axis);

        static float GetSemanticGamepadAxis(SDL_JoystickID instanceId, GamepadAxis axis, float deadZone = 0.3f, bool skipIfHandled = true);

        static void HandleSemanticGamepadAxis(SDL_JoystickID instanceId, GamepadAxis axis);

        static bool IsSemanticGamepadAxisHandled(SDL_JoystickID instanceId, GamepadAxis axis);

        static std::vector<SDL_JoystickID> GetConnectedGamepads();

        static void HandleGamepadButton(SDL_JoystickID instanceId, SDL_GamepadButton button);

        static bool IsGamepadButtonHandled(SDL_JoystickID instanceId, SDL_GamepadButton button);

        static Vec2<float> GetGamepadStick(SDL_JoystickID instanceId, SDL_GamepadAxis xAxis, SDL_GamepadAxis yAxis);

        static Vec2<float> GetSemanticGamepadStick(
            SDL_JoystickID instanceId,
            GamepadAxis xAxis,
            GamepadAxis yAxis,
            float deadZone = 0.3f,
            bool skipIfHandled = true);

        static void RumbleGamepad(SDL_JoystickID instanceId, float lowFrequency, float highFrequency, uint32_t durationMs);

        static void RumbleGamepadTriggers(SDL_JoystickID instanceId, float leftRumble, float rightRumble, uint32_t durationMs);
    };
}
