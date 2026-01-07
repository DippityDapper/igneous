#pragma once

#include "igneous/Vec2.hpp"

struct SDL_Window;

namespace Engine
{
    /// A utility class that holds the SDL window and viewport.
    class Window
    {
    private:
        /// The SDL window.
        static SDL_Window* window;

    public:
        /// The viewport size.
        static Vec2<int> viewport;

    public:
        void Init(int w, int h);
        void Clean();

        /// Gets a pointer to the SDL window.
        /// @returns A pointer to the SDL window.
        static SDL_Window* GetWindow();
    };
}
