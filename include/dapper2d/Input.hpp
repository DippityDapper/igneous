#pragma once

#include <unordered_set>

union SDL_Event;

/// A utility class that handle inputs.
class Input
{
private:
    static std::unordered_set<unsigned int> heldKeys;
    static std::unordered_set<unsigned int> heldMouseButtons;

public:
    /// Handles input events.
    /// @param event The SDL event.
    /// @note Called internally by the engine.
    static void HandleEvents(SDL_Event& event);

    /// Checks whether the given key is down.
    /// @param keyCode The SDLK key code.
    /// @returns true if the key is pressed, false otherwise.
    static bool IsKeyDown(unsigned int keyCode);

    /// Checks whether the given mouse button is down.
    /// @param mouseButton The SDL_BUTTON.
    /// @returns true if the mouse button is pressed, false otherwise.
    static bool IsMouseButtonDown(unsigned int mouseButton);
};
