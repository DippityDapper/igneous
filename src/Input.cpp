#include "igneous/Input.hpp"

#include "SDL3/SDL.h"

std::unordered_set<unsigned int> Input::heldKeys = {};
std::unordered_set<unsigned int> Input::heldMouseButtons = {};

void Input::HandleEvents(SDL_Event &event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        if (!heldKeys.contains(event.key.key))
            heldKeys.insert(event.key.key);
    }
    if (event.type == SDL_EVENT_KEY_UP)
    {
        if (heldKeys.contains(event.key.key))
            heldKeys.erase(event.key.key);
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (!heldMouseButtons.contains(event.button.button))
            heldMouseButtons.insert(event.button.button);
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        if (heldMouseButtons.contains(event.button.button))
            heldMouseButtons.erase(event.button.button);
    }
}

bool Input::IsKeyDown(unsigned int keyCode)
{
    return heldKeys.contains(keyCode);
}

bool Input::IsMouseButtonDown(unsigned int mouseButton)
{
    return heldMouseButtons.contains(mouseButton);
}
