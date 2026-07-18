#include "igneous/input/Input.hpp"

#include <algorithm>
#include <cmath>
#include <ranges>

#include "igneous/input/InputAction.hpp"
#include "igneous/input/InputMap.hpp"
#include "igneous/input/InputMapQuery.hpp"
#include "imgui_internal.h"
#include "SDL3/SDL.h"

#include "igneous/rendering/Window.hpp"

namespace Engine
{
    namespace
    {
        float ClampAxisHalf(float value, bool positive)
        {
            if (positive)
                return std::clamp(value, 0.0f, 1.0f);
            return std::clamp(-value, 0.0f, 1.0f);
        }
    }

    void Input::UpdateSemanticGamepadAxis(SDL_JoystickID instanceId, SDL_GamepadAxis axis, float value)
    {
        auto& axisMap = gamepadSemanticAxisStates[instanceId];

        auto setValue = [&](GamepadAxis semanticAxis, float semanticValue)
        {
            axisMap[semanticAxis].value = semanticValue;
        };

        switch (axis)
        {
            case SDL_GAMEPAD_AXIS_LEFTX:
                if (value >= 0.0f)
                {
                    setValue(GamepadAxis::LeftXRight, ClampAxisHalf(value, true));
                    setValue(GamepadAxis::LeftXLeft, 0.0f);
                }
                else
                {
                    setValue(GamepadAxis::LeftXLeft, ClampAxisHalf(value, false));
                    setValue(GamepadAxis::LeftXRight, 0.0f);
                }
                break;
            case SDL_GAMEPAD_AXIS_LEFTY:
                if (value >= 0.0f)
                {
                    setValue(GamepadAxis::LeftYDown, ClampAxisHalf(value, true));
                    setValue(GamepadAxis::LeftYUp, 0.0f);
                }
                else
                {
                    setValue(GamepadAxis::LeftYUp, ClampAxisHalf(value, false));
                    setValue(GamepadAxis::LeftYDown, 0.0f);
                }
                break;
            case SDL_GAMEPAD_AXIS_RIGHTX:
                if (value >= 0.0f)
                {
                    setValue(GamepadAxis::RightXRight, ClampAxisHalf(value, true));
                    setValue(GamepadAxis::RightXLeft, 0.0f);
                }
                else
                {
                    setValue(GamepadAxis::RightXLeft, ClampAxisHalf(value, false));
                    setValue(GamepadAxis::RightXRight, 0.0f);
                }
                break;
            case SDL_GAMEPAD_AXIS_RIGHTY:
                if (value >= 0.0f)
                {
                    setValue(GamepadAxis::RightYDown, ClampAxisHalf(value, true));
                    setValue(GamepadAxis::RightYUp, 0.0f);
                }
                else
                {
                    setValue(GamepadAxis::RightYUp, ClampAxisHalf(value, false));
                    setValue(GamepadAxis::RightYDown, 0.0f);
                }
                break;
            case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                setValue(GamepadAxis::TriggerLeft, std::clamp(value, 0.0f, 1.0f));
                break;
            case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                setValue(GamepadAxis::TriggerRight, std::clamp(value, 0.0f, 1.0f));
                break;
            default:
                break;
        }
    }

    bool Input::Init()
    {
        InitGamepads();
        RestoreBaseline();
        return true;
    }

    void Input::ResetForTests()
    {
        for (auto& [id, gamepad]: gamepads)
        {
            if (gamepad)
                SDL_CloseGamepad(gamepad);
        }

        layers.clear();
        keyEvents.clear();
        mouseEvents.clear();
        gamepads.clear();
        gamepadButtonEvents.clear();
        gamepadAxisValues.clear();
        gamepadSemanticAxisStates.clear();
        inputMap.reset();

        mouseX = 0;
        mouseY = 0;
        mouseVelX = 0;
        mouseVelY = 0;
        mouseWheelVelX = 0;
        mouseWheelVelY = 0;
    }

    void Input::RestoreBaseline()
    {
        if (!layers.contains("_default"))
            AddInputLayer("_default", 0);
    }

    bool Input::InitGamepads()
    {
        int count = 0;
        SDL_JoystickID* joystickIds = SDL_GetGamepads(&count);
        if (joystickIds)
        {
            for (int i = 0; i < count; i++)
            {
                SDL_JoystickID id = joystickIds[i];
                SDL_Gamepad* gamepad = SDL_OpenGamepad(id);
                if (gamepad)
                {
                    gamepads[id] = gamepad;
                }
            }
            SDL_free(joystickIds);
        }
        return true;
    }

    void Input::ResetEvents()
    {
        mouseVelX = 0;
        mouseVelY = 0;
        mouseWheelVelX = 0;
        mouseWheelVelY = 0;

        for (auto& keyEvent: keyEvents | std::views::values)
        {
            keyEvent.pressedLastFrame = keyEvent.pressed;
            keyEvent.handled = false;
        }
        for (auto& mouseEvent: mouseEvents | std::views::values)
        {
            mouseEvent.pressedLastFrame = mouseEvent.pressed;
            mouseEvent.handled = false;
        }
        for (auto& buttonMap: gamepadButtonEvents | std::views::values)
        {
            for (auto& event: buttonMap | std::views::values)
            {
                event.pressedLastFrame = event.pressed;
                event.handled = false;
            }
        }
        for (auto& axisMap: gamepadSemanticAxisStates | std::views::values)
        {
            for (auto& state: axisMap | std::views::values)
                state.handled = false;
        }
    }

    void Input::HandleEvent(SDL_Event& event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
        {
            if (!keyEvents.contains(event.key.key))
            {
                InputEvent inputEvent{};
                keyEvents.emplace(event.key.key, inputEvent);
            }
            InputEvent& keyEvent = keyEvents[event.key.key];
            keyEvent.pressed = event.key.down;
            if (ImGui::GetIO().WantCaptureKeyboard)
                keyEvent.handled = true;
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            if (!mouseEvents.contains(event.button.button))
            {
                InputEvent inputEvent{};
                mouseEvents.emplace(event.button.button, inputEvent);
            }
            InputEvent& mouseEvent = mouseEvents[event.button.button];
            mouseEvent.pressed = event.button.down;
            if (ImGui::GetIO().WantCaptureMouse)
                mouseEvent.handled = true;
        }
        if (event.type == SDL_EVENT_MOUSE_MOTION)
        {
            mouseX = event.motion.x;
            mouseY = event.motion.y;
            mouseVelX = event.motion.xrel;
            mouseVelY = event.motion.yrel;
        }
        if (event.type == SDL_EVENT_MOUSE_WHEEL)
        {
            mouseWheelVelX = event.wheel.x;
            mouseWheelVelY = event.wheel.y;
        }
        if (event.type == SDL_EVENT_GAMEPAD_ADDED)
        {
            SDL_JoystickID id = event.gdevice.which;
            if (!gamepads.contains(id))
            {
                SDL_Gamepad* gamepad = SDL_OpenGamepad(id);
                if (gamepad)
                {
                    gamepads[id] = gamepad;
                }
            }
        }
        if (event.type == SDL_EVENT_GAMEPAD_REMOVED)
        {
            SDL_JoystickID id = event.gdevice.which;
            if (gamepads.contains(id))
            {
                SDL_CloseGamepad(gamepads[id]);
                gamepads.erase(id);
                gamepadButtonEvents.erase(id);
                gamepadAxisValues.erase(id);
                gamepadSemanticAxisStates.erase(id);
            }
        }
        if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN || event.type == SDL_EVENT_GAMEPAD_BUTTON_UP)
        {
            SDL_JoystickID id = event.gbutton.which;
            SDL_GamepadButton button = static_cast<SDL_GamepadButton>(event.gbutton.button);
            InputEvent& btnEvent = gamepadButtonEvents[id][button];
            btnEvent.pressed = event.gbutton.down;
        }
        if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION)
        {
            SDL_JoystickID id = event.gaxis.which;
            SDL_GamepadAxis axis = static_cast<SDL_GamepadAxis>(event.gaxis.axis);
            float value = static_cast<float>(event.gaxis.value) / 32767.0f;
            gamepadAxisValues[id][axis] = value;
            UpdateSemanticGamepadAxis(id, axis, value);
        }
    }

    bool Input::AddInputLayer(const std::string& layerName, int priority)
    {
        if (layers.contains(layerName))
        {
            SDL_Log("Layer %s already exists", layerName.c_str());
            return false;
        }

        layers.emplace(layerName, std::make_unique<InputLayer>(layerName, priority));
        return true;
    }

    bool Input::RemoveInputLayer(const std::string& layerName)
    {
        if (!layers.contains(layerName))
            return false;

        layers.erase(layerName);
        return true;
    }

    std::vector<InputLayer*> Input::GetInputLayers()
    {
        std::vector<InputLayer*> sortedLayers{};
        sortedLayers.reserve(layers.size());
        for (const auto& layer: layers | std::views::values)
            sortedLayers.push_back(layer.get());

        std::ranges::sort(sortedLayers, [](const InputLayer* a, const InputLayer* b)
        {
            return a->GetPriority() > b->GetPriority();
        });
        return sortedLayers;
    }

    void Input::SetInputMap(std::shared_ptr<InputMap> map)
    {
        inputMap = std::move(map);
    }

    InputMap* Input::GetInputMap()
    {
        return inputMap.get();
    }

    InputAction* Input::GetInputAction(const std::string& actionName)
    {
        if (!inputMap)
            return nullptr;
        return inputMap->FindAction(actionName);
    }

    float Input::GetAction(const std::string& actionName)
    {
        return inputMap ? inputMap->GetValue(actionName) : 0.0f;
    }

    bool Input::IsActionPressed(const std::string& actionName)
    {
        return inputMap ? inputMap->IsPressed(actionName) : false;
    }

    bool Input::IsActionJustPressed(const std::string& actionName)
    {
        return inputMap ? inputMap->IsJustPressed(actionName) : false;
    }

    bool Input::IsActionJustReleased(const std::string& actionName)
    {
        return inputMap ? inputMap->IsJustReleased(actionName) : false;
    }

    float Input::GetAxis(const std::string& positiveAction, const std::string& negativeAction)
    {
        return InputMapQuery::GetAxis(positiveAction, negativeAction);
    }

    void Input::HandleAllKeys(const std::vector<SDL_Keycode>* exceptions)
    {
        for (auto& [key, event]: keyEvents)
        {
            if (exceptions)
            {
                if (std::ranges::find(*exceptions, key) != exceptions->end())
                    continue;
            }
            event.handled = true;
        }
    }

    void Input::HandleAllButtons(const std::vector<SDL_MouseButtonFlags>* exceptions)
    {
        for (auto& [button, event]: mouseEvents)
        {
            if (exceptions)
            {
                if (std::ranges::find(*exceptions, button) != exceptions->end())
                    continue;
            }
            event.handled = true;
        }
    }

    void Input::HandleAllGamepadButtons(const std::vector<SDL_GamepadButton>* exceptions)
    {
        for (auto& [deviceId, buttonMap]: gamepadButtonEvents)
        {
            (void) deviceId;
            for (auto& [button, event]: buttonMap)
            {
                if (exceptions)
                {
                    if (std::ranges::find(*exceptions, button) != exceptions->end())
                        continue;
                }
                event.handled = true;
            }
        }
    }

    void Input::HandleAllGamepadAxes(const std::vector<GamepadAxis>* exceptions)
    {
        for (auto& [deviceId, axisMap]: gamepadSemanticAxisStates)
        {
            (void) deviceId;
            for (auto& [axis, state]: axisMap)
            {
                if (exceptions)
                {
                    if (std::ranges::find(*exceptions, axis) != exceptions->end())
                        continue;
                }
                state.handled = true;
            }
        }
    }

    void Input::HandleAllInputs(
        const std::vector<SDL_Keycode>* keyExceptions,
        const std::vector<SDL_MouseButtonFlags>* mouseButtonExceptions,
        const std::vector<SDL_GamepadButton>* gamepadButtonExceptions,
        const std::vector<GamepadAxis>* gamepadAxisExceptions)
    {
        HandleAllKeys(keyExceptions);
        HandleAllButtons(mouseButtonExceptions);
        HandleAllGamepadButtons(gamepadButtonExceptions);
        HandleAllGamepadAxes(gamepadAxisExceptions);
    }

    void Input::HandleKey(SDL_Keycode key)
    {
        if (!keyEvents.contains(key))
            return;
        keyEvents[key].handled = true;
    }

    void Input::HandleButton(SDL_MouseButtonFlags mouseButton)
    {
        if (!mouseEvents.contains(mouseButton))
            return;
        mouseEvents[mouseButton].handled = true;
    }

    bool Input::IsKeyHandled(SDL_Keycode key)
    {
        if (!keyEvents.contains(key))
            return false;
        return keyEvents[key].handled;
    }

    bool Input::IsButtonHandled(SDL_MouseButtonFlags mouseButton)
    {
        if (!mouseEvents.contains(mouseButton))
            return false;
        return mouseEvents[mouseButton].handled;
    }

    bool Input::IsKeyDown(SDL_Keycode key, bool skipIfHandled)
    {
        if (!keyEvents.contains(key))
            return false;
        if (skipIfHandled && keyEvents[key].handled)
            return false;
        if (!keyEvents[key].pressed)
            return false;
        HandleKey(key);
        return true;
    }

    bool Input::IsKeyJustPressed(SDL_Keycode key, bool skipIfHandled)
    {
        if (!keyEvents.contains(key))
            return false;
        if (skipIfHandled && keyEvents[key].handled)
            return false;
        if (!keyEvents[key].pressed || keyEvents[key].pressedLastFrame)
            return false;
        HandleKey(key);
        return true;
    }

    bool Input::IsKeyJustReleased(SDL_Keycode key)
    {
        if (!keyEvents.contains(key))
            return false;
        if (keyEvents[key].pressed || !keyEvents[key].pressedLastFrame)
            return false;
        return true;
    }

    bool Input::IsMouseButtonDown(SDL_MouseButtonFlags mouseButton, bool skipIfHandled)
    {
        if (!mouseEvents.contains(mouseButton))
            return false;
        if (skipIfHandled && mouseEvents[mouseButton].handled)
            return false;
        if (!mouseEvents[mouseButton].pressed)
            return false;
        HandleButton(mouseButton);
        return true;
    }

    bool Input::IsButtonJustPressed(SDL_MouseButtonFlags mouseButton, bool skipIfHandled)
    {
        if (!mouseEvents.contains(mouseButton))
            return false;
        if (skipIfHandled && mouseEvents[mouseButton].handled)
            return false;
        if (!mouseEvents[mouseButton].pressed || mouseEvents[mouseButton].pressedLastFrame)
            return false;
        HandleButton(mouseButton);
        return true;
    }

    bool Input::IsButtonJustReleased(SDL_MouseButtonFlags mouseButton)
    {
        if (!mouseEvents.contains(mouseButton))
            return false;
        if (mouseEvents[mouseButton].pressed || !mouseEvents[mouseButton].pressedLastFrame)
            return false;
        return true;
    }

    bool Input::IsWindowResized()
    {
        return Window::WasResized();
    }

    Vec2<float> Input::GetMouseScreenPosition()
    {
        return {mouseX, mouseY};
    }

    Vec2<float> Input::GetMouseVelocity()
    {
        return {mouseVelX, mouseVelY};
    }

    Vec2<float> Input::GetMouseWheelVelocity()
    {
        return {mouseWheelVelX, mouseWheelVelY};
    }

    void Input::HandleGamepadButton(SDL_JoystickID instanceId, SDL_GamepadButton button)
    {
        if (!gamepadButtonEvents.contains(instanceId))
            return;
        if (!gamepadButtonEvents[instanceId].contains(button))
            return;
        gamepadButtonEvents[instanceId][button].handled = true;
    }

    bool Input::IsGamepadButtonHandled(SDL_JoystickID instanceId, SDL_GamepadButton button)
    {
        if (!gamepadButtonEvents.contains(instanceId))
            return false;
        if (!gamepadButtonEvents[instanceId].contains(button))
            return false;
        return gamepadButtonEvents[instanceId][button].handled;
    }

    bool Input::IsGamepadButtonDown(SDL_JoystickID instanceId, SDL_GamepadButton button, bool skipIfHandled)
    {
        if (!gamepadButtonEvents.contains(instanceId))
            return false;
        auto& btnMap = gamepadButtonEvents[instanceId];
        if (!btnMap.contains(button))
            return false;
        if (skipIfHandled && btnMap[button].handled)
            return false;
        if (!btnMap[button].pressed)
            return false;
        HandleGamepadButton(instanceId, button);
        return true;
    }

    bool Input::IsGamepadButtonJustPressed(SDL_JoystickID instanceId, SDL_GamepadButton button, bool skipIfHandled)
    {
        if (!gamepadButtonEvents.contains(instanceId))
            return false;
        auto& btnMap = gamepadButtonEvents[instanceId];
        if (!btnMap.contains(button))
            return false;
        if (skipIfHandled && btnMap[button].handled)
            return false;
        if (!btnMap[button].pressed || btnMap[button].pressedLastFrame)
            return false;
        HandleGamepadButton(instanceId, button);
        return true;
    }

    bool Input::IsGamepadButtonJustReleased(SDL_JoystickID instanceId, SDL_GamepadButton button)
    {
        if (!gamepadButtonEvents.contains(instanceId))
            return false;
        auto& btnMap = gamepadButtonEvents[instanceId];
        if (!btnMap.contains(button))
            return false;
        if (btnMap[button].pressed || !btnMap[button].pressedLastFrame)
            return false;
        return true;
    }

    float Input::GetGamepadAxis(SDL_JoystickID instanceId, SDL_GamepadAxis axis)
    {
        if (!gamepadAxisValues.contains(instanceId))
            return 0.0f;
        auto& axisMap = gamepadAxisValues[instanceId];
        if (!axisMap.contains(axis))
            return 0.0f;
        return axisMap[axis];
    }

    void Input::HandleSemanticGamepadAxis(SDL_JoystickID instanceId, GamepadAxis axis)
    {
        if (!gamepadSemanticAxisStates.contains(instanceId))
            return;
        if (!gamepadSemanticAxisStates[instanceId].contains(axis))
            return;
        gamepadSemanticAxisStates[instanceId][axis].handled = true;
    }

    bool Input::IsSemanticGamepadAxisHandled(SDL_JoystickID instanceId, GamepadAxis axis)
    {
        if (!gamepadSemanticAxisStates.contains(instanceId))
            return false;
        auto& axisMap = gamepadSemanticAxisStates[instanceId];
        if (!axisMap.contains(axis))
            return false;
        return axisMap[axis].handled;
    }

    float Input::GetSemanticGamepadAxis(SDL_JoystickID instanceId, GamepadAxis axis, float deadZone, bool skipIfHandled)
    {
        if (!gamepadSemanticAxisStates.contains(instanceId))
            return 0.0f;
        auto& axisMap = gamepadSemanticAxisStates[instanceId];
        if (!axisMap.contains(axis))
            return 0.0f;

        SemanticAxisState& state = axisMap[axis];
        if (skipIfHandled && state.handled)
            return 0.0f;
        if (std::abs(state.value) <= deadZone)
            return 0.0f;

        HandleSemanticGamepadAxis(instanceId, axis);
        return state.value;
    }

    Vec2<float> Input::GetSemanticGamepadStick(
        SDL_JoystickID instanceId,
        GamepadAxis xAxis,
        GamepadAxis yAxis,
        float deadZone,
        bool skipIfHandled)
    {
        return {
            GetSemanticGamepadAxis(instanceId, xAxis, deadZone, skipIfHandled),
            GetSemanticGamepadAxis(instanceId, yAxis, deadZone, skipIfHandled),
        };
    }

    std::vector<SDL_JoystickID> Input::GetConnectedGamepads()
    {
        std::vector<SDL_JoystickID> ids;
        ids.reserve(gamepads.size());
        for (auto id: gamepads | std::views::keys)
            ids.push_back(id);
        return ids;
    }

    Vec2<float> Input::GetGamepadStick(SDL_JoystickID instanceId, SDL_GamepadAxis xAxis, SDL_GamepadAxis yAxis)
    {
        return {GetGamepadAxis(instanceId, xAxis), GetGamepadAxis(instanceId, yAxis)};
    }

    void Input::RumbleGamepad(SDL_JoystickID instanceId, float lowFrequency, float highFrequency, uint32_t durationMs)
    {
        if (!gamepads.contains(instanceId))
            return;

        if (lowFrequency < 0)
            lowFrequency = 0;
        if (highFrequency < 0)
            highFrequency = 0;
        if (lowFrequency > 1)
            lowFrequency = 1;
        if (highFrequency > 1)
            highFrequency = 1;
 uint16_t lf = static_cast<uint16_t>(lowFrequency * UINT16_MAX);
        uint16_t rf = static_cast<uint16_t>(highFrequency * UINT16_MAX);
        SDL_RumbleGamepad(gamepads[instanceId], lf, rf, durationMs);
    }

    void Input::RumbleGamepadTriggers(SDL_JoystickID instanceId, float leftRumble, float rightRumble, uint32_t durationMs)
    {
        if (!gamepads.contains(instanceId))
            return;

        if (leftRumble < 0)
            leftRumble = 0;
        if (rightRumble < 0)
            rightRumble = 0;
        if (leftRumble > 1)
            leftRumble = 1;
        if (rightRumble > 1)
            rightRumble = 1;

        uint16_t lr = static_cast<uint16_t>(leftRumble * UINT16_MAX);
        uint16_t rr = static_cast<uint16_t>(rightRumble * UINT16_MAX);
        SDL_RumbleGamepadTriggers(gamepads[instanceId], lr, rr, durationMs);
    }
}
