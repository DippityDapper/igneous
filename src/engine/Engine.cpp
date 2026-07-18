#include "igneous/engine/Engine.hpp"

#include <filesystem>
#include <ranges>

#include "imgui_impl_sdl3.h"
#include "enet/enet.h"
#include "SDL3/SDL_filesystem.h"
#include "SDL3_mixer/SDL_mixer.h"

#include "igneous/rendering/Renderer.hpp"
#include "igneous/rendering/Window.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/engine/Camera.hpp"
#include "igneous/engine/Time.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/scenes/SceneManager.hpp"

#ifdef IGNEOUS_STEAM_ENABLED
#include "igneous/networking/SteamBootstrap.hpp"
#endif

namespace Engine
{
    void Engine::Init()
    {
        running = true;
        initState = {};

        if (!InitSDL())
        {
            running = false;
            return;
        }

        if (!InitENet())
        {
            running = false;
            return;
        }

        if (!Input::Init())
        {
            running = false;
            return;
        }
        initState.input = true;

        if (!SceneManager::Init())
        {
            running = false;
            return;
        }
        initState.sceneManager = true;
    }

    bool Engine::InitSDL()
    {
#if defined(IGNEOUS_BUILD_TESTS)
        if (TestHooks::forceInitSDLFailure)
        {
            SDL_Log("SDL init failed: (test hook)");
            return false;
        }
#endif

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD))
        {
            SDL_Log("SDL init failed: %s", SDL_GetError());
            return false;
        }
        initState.sdl = true;

        if (const char* basePath = SDL_GetBasePath())
        {
            std::error_code ec;
            std::filesystem::current_path(basePath, ec);
            if (ec)
                SDL_Log("Failed to set working directory to %s: %s", basePath, ec.message().c_str());
        }

        if (!MIX_Init())
        {
            SDL_Log("MIX init failed: %s", SDL_GetError());
            SDL_Quit();
            initState.sdl = false;
            return false;
        }
        initState.mix = true;

        Window::Init(initSettings.width, initSettings.height, initSettings.title);
        Renderer::Init();

        Time::currentTick = SDL_GetTicks();
        Time::lastTick = Time::currentTick;

        return true;
    }

    bool Engine::InitENet()
    {
        if (enet_initialize() < 0)
        {
            SDL_Log("Network init failed: %s", SDL_GetError());
            return false;
        }
        initState.enet = true;

        return true;
    }

    void Engine::HandleEvents()
    {
        Window::ResetFrameState();
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
                Window::OnResize(sdlEvent.window.data1, sdlEvent.window.data2);
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

#ifdef IGNEOUS_STEAM_ENABLED
            SteamBootstrap::RunCallbacks();
#endif

            if (SceneManager::GetSceneRoot())
                SceneManager::GetSceneRoot()->Update(Time::deltaTime);
            if (Camera::main)
                Camera::main->UpdateInternal(Time::deltaTime);
            ResourceManager::CleanExpired(10);

            if (SceneManager::GetSceneRoot())
                SceneManager::GetSceneRoot()->Render();
            ResourceManager::RenderSprites(Time::deltaTime);

            Renderer::Render();
            if (SceneManager::GetSceneRoot())
                SceneManager::GetSceneRoot()->ProcessRemoveScenesQueue();
        }
    }

    void Engine::Clean() const
    {
        if (initState.sceneManager && SceneManager::GetSceneRoot())
            SceneManager::GetSceneRoot()->Clean();

        if (initState.sceneManager)
            SceneManager::RemoveSceneRoot();

        if (initState.input)
        {
            Input::ResetForTests();
            Input::RestoreBaseline();
        }

        Camera::ResetForTests();
        Time::ResetForTests();

        if (initState.sdl)
            ResourceManager::Clean();

        if (initState.enet)
            enet_deinitialize();

#ifdef IGNEOUS_STEAM_ENABLED
        if (initState.sdl)
            SteamBootstrap::Shutdown();
#endif

        if (initState.sdl)
        {
            Renderer::Clean();
            Window::Clean();
            SDL_Quit();
        }
    }

    void Engine::Quit()
    {
        running = false;
    }

#if defined(IGNEOUS_BUILD_TESTS)
    void Engine::ResetForTests()
    {
        running = true;
        initState = {};
        TestHooks::Reset();
    }
#endif
}
