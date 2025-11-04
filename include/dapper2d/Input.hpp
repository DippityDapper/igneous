#pragma once

#include <unordered_set>

union SDL_Event;

class Input
{
private:
    static std::unordered_set<unsigned int> heldKeys;
    static std::unordered_set<unsigned int> heldMouseButtons;

public:
    static void HandleEvents(SDL_Event& event);
    static bool IsKeyDown(unsigned int keyCode);
    static bool IsMouseButtonDown(unsigned int mouseButton);
};
