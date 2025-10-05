#pragma once

#include "SDL3/SDL.h"

#include "dapper2d/Vec2.hpp"

namespace Engine
{
    class Window
    {
    public:
        static SDL_Window* window;
        static Vec2<int> viewport;

    public:
        void Init(int w, int h);
        void Clean();

        static SDL_Window* GetWindow();
    };
}
