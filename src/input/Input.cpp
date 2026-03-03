#include "igneous/input/Input.hpp"

#include <ranges>

#include "imgui_internal.h"
#include "SDL3/SDL.h"

namespace Engine
{
    bool Input::Init()
    {
        AddInputLayer("gameplay", 0);
        AddInputLayer("ui", 1);
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
    }

    bool Input::AddInputLayer(const std::string& layerName, int priority)
    {
        if (layerLookup.contains(layerName))
        {
            SDL_Log("Layer %s already exists", layerName.c_str());
            return false;
        }

        InputLayer inputLayer{layerName, priority};
        auto it = layers.emplace(priority, inputLayer);
        layerLookup[layerName] = it;
        return true;
    }

    bool Input::RemoveInputLayer(const std::string& layerName)
    {
        auto it = layerLookup.find(layerName);
        if (it == layerLookup.end())
        {
            SDL_Log("Layer %s does not exist", layerName.c_str());
            return false;
        }

        layers.erase(it->second);
        layerLookup.erase(it);
        return true;
    }

    std::multimap<int, InputLayer, std::greater<>> Input::GetInputLayers()
    {
        return layers;
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
}
