#pragma once

#include <map>

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

#include "engine/packets/Packets.h"
#include "engine/parser/CFGParser.h"

#include "engine/entity/ClientEntity.h"
#include "engine/networking/NetworkManager.h"
#include "engine/world/World.h"
#include "engine/engine/Scene.h"
#include "engine/engine/Renderer.h"
#include "engine/engine/Window.h"

namespace Engine
{
    class Engine
    {
    private:
        bool running = true;
        NetworkManager networkManager{};

        Scene* scene = nullptr;

    public:
        Window window;
        Renderer renderer;

        uint64_t lastTick = 0;
        uint64_t currentTick = 0;
        float deltaTime = 0;

    private:
        SDL_AppResult InitSDL();
        SDL_AppResult InitNetworking();

    public:
        void Init();
        void Update();
        void Render();
        void Clean();
        void HandleEvents();

        void SetScene(Scene* _scene);
    };
}
