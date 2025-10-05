#pragma once

#include "SDL3/SDL.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

#include "engine/vector/Vec2.h"

#include "engine/engine/Window.h"
#include "engine/camera/Camera.h"
#include "engine/entity/Sprite.h"

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
