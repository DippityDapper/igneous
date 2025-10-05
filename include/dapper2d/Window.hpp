#pragma once

#include "dapper2d/Vec2.hpp"

struct SDL_Window;

namespace Engine
{
    class Window
    {
    private:
        static SDL_Window* window;

    public:
        static Vec2<int> viewport;

    public:
        void Init(int w, int h);
        void Clean();

        static SDL_Window* GetWindow();
    };
}
