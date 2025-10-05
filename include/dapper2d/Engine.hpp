#pragma once

#include <map>

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

#include "dapper2d/Packets.hpp"
#include "dapper2d/CFGParser.hpp"
#include "dapper2d/Scene.hpp"
#include "dapper2d/Renderer.hpp"
#include "dapper2d/Window.hpp"
#include "dapper2d/ResourceLoader.hpp"

namespace Engine
{
    class Engine
    {
    private:
        bool running = true;

        static Scene* scene;

    public:
        Window window;
        Renderer renderer;

        uint64_t lastTick = 0;
        uint64_t currentTick = 0;
        float deltaTime = 0;

    private:
        void Render();
        void HandleEvents();
        SDL_AppResult InitSDL();

    public:
        void Init(Scene* _scene);
        void Update();
        void Clean();
        static void SetScene(Scene* _scene);
    };
}
