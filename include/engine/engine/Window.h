#pragma once

#include "SDL3/SDL.h"

#include "engine/vector/Vec2.h"

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
