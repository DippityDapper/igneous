#pragma once

#include "SDL3/SDL.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

#include "dapper2d/Vec2.hpp"
#include "dapper2d/Window.hpp"
#include "dapper2d/Camera.hpp"
#include "dapper2d/Sprite.hpp"

namespace Engine
{
    class Renderer
    {
    private:
        static SDL_Renderer* renderer;

    public:
        void Init();
        void Render();
        void Clean();

        static void BufferClear();
        static void BufferAdd(Vec2<float> position, Sprite* sprite);
        static SDL_Renderer* GetRenderer();
    };
}
