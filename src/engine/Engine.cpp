#include "igneous/engine/Engine.hpp"

#include <ranges>

#include "imgui_impl_sdl3.h"
#include "enet/enet.h"
#include "SDL3_mixer/SDL_mixer.h"

#include "igneous/rendering/Renderer.hpp"
#include "igneous/rendering/Window.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/engine/Camera.hpp"
#include "igneous/engine/Time.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/scenes/SceneManager.hpp"

namespace Engine
{
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

        if (!Input::Init())
        {
            running = false;
        }

        if (!SceneManager::Init())
        {
            running = false;
        }
    }

    bool Engine::InitSDL() const
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        {
            SDL_Log("SDL init failed: %s", SDL_GetError());
            return false;
        }

        if (!MIX_Init())
        {
            SDL_Log("MIX init failed: %s", SDL_GetError());
            return false;
        }

        Window::Init(640, 360);
        Renderer::Init();

        Time::currentTick = SDL_GetTicks();
        Time::lastTick = Time::currentTick;

        return true;
    }

    bool Engine::InitENet() const
    {
        if (enet_initialize() < 0)
        {
            SDL_Log("Network init failed: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    void Engine::HandleEvents()
    {
        Input::ResetEvents();

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

            Input::HandleEvent(sdlEvent);
        }

        for (const auto& layer: Input::GetInputLayers())
        {
            if (Camera::main)
                Camera::main->HandleEventsInternal(*layer);
            if (SceneManager::GetSceneRoot())
                SceneManager::GetSceneRoot()->HandleEvents(*layer);
        }
    }

    void Engine::Update()
    {
        while (running)
        {
            Renderer::BufferClear();

            Time::lastTick = Time::currentTick;
            Time::currentTick = SDL_GetTicks();
            Time::deltaTime = (double) (Time::currentTick - Time::lastTick) / 1000;

            HandleEvents();

            if (SceneManager::GetSceneRoot())
                SceneManager::GetSceneRoot()->Update(Time::deltaTime);
            if (Camera::main)
                Camera::main->UpdateInternal(Time::deltaTime);
            ResourceManager::CleanExpired(10);

            if (SceneManager::GetSceneRoot())
                SceneManager::GetSceneRoot()->Render();
            ResourceManager::RenderSprites(Time::deltaTime);

            Renderer::Render();
        }
    }

    void Engine::Clean() const
    {
        if (SceneManager::GetSceneRoot())
            SceneManager::GetSceneRoot()->Clean();
        ResourceManager::Clean();
        enet_deinitialize();

        Renderer::Clean();
        Window::Clean();

        SDL_Quit();
    }

    void Engine::Quit()
    {
        running = false;
    }
}
