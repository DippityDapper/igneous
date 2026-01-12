#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "igneous/engine/Vec2.hpp"
#include "igneous/input/InputEvent.hpp"
#include "igneous/input/InputLayer.hpp"
#include "SDL3/SDL_events.h"

namespace Engine
{
    class Input
    {
    private:
        static inline std::multimap<int, InputLayer, std::greater<>> layers{};
        static inline std::unordered_map<std::string, std::multimap<int, InputLayer, std::greater<>>::iterator> layerLookup{};

        static inline std::unordered_map<SDL_Keycode, InputEvent> keyEvents{};
        static inline std::unordered_map<SDL_MouseButtonFlags, InputEvent> mouseEvents{};

        static inline float mouseX = 0;
        static inline float mouseY = 0;
        static inline float mouseVelX = 0;
        static inline float mouseVelY = 0;
        static inline float mouseWheelVelX = 0;
        static inline float mouseWheelVelY = 0;

        static inline bool wasWindowResized = false;

    public:
        static bool Init();

        static void ResetEvents();

        /// Handles input events.
        /// @param event The SDL event.
        /// @note Called internally by the engine.
        static void HandleEvent(SDL_Event& event);

        static bool AddInputLayer(const std::string& layerName, int priority);
        static bool RemoveInputLayer(const std::string& layerName);
        static std::multimap<int, InputLayer, std::greater<>> GetInputLayers();

        static void HandleKey(SDL_Keycode key);
        static void HandleButton(SDL_MouseButtonFlags mouseButton);
        static bool IsKeyHandled(SDL_Keycode key);
        static bool IsButtonHandled(SDL_MouseButtonFlags mouseButton);

        static bool IsKeyDown(SDL_Keycode key, bool skipIfHandled=true);
        static bool IsKeyJustPressed(SDL_Keycode key, bool skipIfHandled=true);
        static bool IsKeyJustReleased(SDL_Keycode key);

        static bool IsMouseButtonDown(SDL_MouseButtonFlags mouseButton, bool skipIfHandled=true);
        static bool IsButtonJustPressed(SDL_MouseButtonFlags mouseButton, bool skipIfHandled=true);
        static bool IsButtonJustReleased(SDL_MouseButtonFlags mouseButton);

        static bool IsWindowResized();

        static Vec2<float> GetMouseScreenPosition();
        static Vec2<float> GetMouseVelocity();
        static Vec2<float> GetMouseWheelVelocity();
    };
}
