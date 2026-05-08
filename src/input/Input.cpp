#include "igneous/input/Input.hpp"

#include <ranges>

#include "imgui_internal.h"
#include "SDL3/SDL.h"

namespace Engine
{
    bool Input::Init()
    {
        InitGamepads();
        AddInputLayer("_default", 0);
        return true;
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
                    SDL_Log("Gamepad connected: %s (id %d)", SDL_GetGamepadName(gamepad), id);
                }
            }
            SDL_free(joystickIds);
        }
        return true;
    }

    void Input::ResetEvents()
    {
        wasWindowResized = false;
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
        if (event.type == SDL_EVENT_WINDOW_RESIZED)
        {
            wasWindowResized = true;
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
                    SDL_Log("Gamepad connected: %s (id %d)", SDL_GetGamepadName(gamepad), id);
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
                SDL_Log("Gamepad disconnected (id %d)", id);
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
            // SDL axis range is -32768 to 32767; normalize to -1.0..1.0
            // Triggers report 0 to 32767, so they map to 0.0..1.0 correctly
            gamepadAxisValues[id][axis] = event.gaxis.value / 32767.0f;
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
        std::vector<InputLayer*> _layers{};
        for (const auto& layer: layers | std::views::values)
        {
            _layers.push_back(layer.get());
        }
        return _layers;
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
        return wasWindowResized;
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
}
