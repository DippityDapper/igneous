#include "dapper2d/Engine.hpp"

#include "imgui_impl_sdl3.h"

#include "dapper2d/Scene.hpp"
#include "dapper2d/Renderer.hpp"
#include "dapper2d/Window.hpp"
#include "dapper2d/ResourceLoader.hpp"
#include "dapper2d/Camera.hpp"
#include "dapper2d/Input.hpp"
#include "dapper2d/Networking.hpp"
#include "dapper2d/Scenes.hpp"

namespace Engine
{
    bool Engine::running = true;
    uint64_t Engine::lastTick = 0;
    uint64_t Engine::currentTick = 0;
    float Engine::deltaTime = 0;

    int Engine::Run()
    {
        Init();
        Update();
        Clean();
        return 0;
    }

    void Engine::Init()
    {
        if (!InitSDL())
        {
            running = false;
            return;
        }

        if (!InitENet())
        {
            running = false;
        }
    }

    bool Engine::InitSDL() const
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("SDL init failed: %s", SDL_GetError());
            return false;
        }

        window->Init(640, 360);
        renderer->Init();

        currentTick = SDL_GetTicks();
        lastTick = currentTick;

        return true;
    }

    bool Engine::InitENet() const
    {
        if (!Networking::Init())
        {
            SDL_Log("Network init failed: %s", SDL_GetError());
            return false;
        }

        return true;
    }


    void Engine::HandleEvents()
    {
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent))
        {
            ImGui_ImplSDL3_ProcessEvent(&sdlEvent);

            if (sdlEvent.type == SDL_EVENT_QUIT)
            {
                Quit();
            }
            if (sdlEvent.type == SDL_EVENT_WINDOW_RESIZED)
            {
                Window::viewport.x = sdlEvent.window.data1;
                Window::viewport.y = sdlEvent.window.data2;
            }

            Input::HandleEvents(sdlEvent);
            if (Camera::main)
                Camera::main->HandleEventsInternal(sdlEvent);
            for (const auto& kvp : Scenes::loadedScenes)
            {
                if (kvp.second)
                {
                    kvp.second->HandleEventsInternal(sdlEvent);
                }
            }
        }
    }

    void Engine::Update()
    {
        while (running)
        {
            Scenes::LoadQueue();
            HandleEvents();

            lastTick = currentTick;
            currentTick = SDL_GetTicks();
            deltaTime = (float)(currentTick - lastTick) / 1000.0f;

            for (const auto& kvp : Scenes::loadedScenes)
            {
                if (kvp.second)
                {
                    kvp.second->UpdateInternal(deltaTime);
                }
            }
            if (Camera::main)
                Camera::main->UpdateInternal(deltaTime);

            ResourceLoader::CleanExpired(10);
            Render();
        }
    }

    void Engine::Render() const
    {
        Renderer::BufferClear();
        for (const auto& kvp : Scenes::loadedScenes)
        {
            if (kvp.second)
            {
                kvp.second->RenderInternal();
            }
        }
        renderer->Render();
    }

    void Engine::Clean() const
    {
        Scenes::Clean();
        Networking::Clean();

        renderer->Clean();
        window->Clean();

        delete renderer;
        delete window;

        SDL_Quit();
    }

    float Engine::GetDeltaTime()
    {
        return deltaTime;
    }

    void Engine::Quit()
    {
        running = false;
    }
}
