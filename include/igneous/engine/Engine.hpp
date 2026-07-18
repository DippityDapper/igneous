// Doc: docs/classes/Engine.md
#pragma once

#include <string>

#include "igneous/scenes/Scene.hpp"
#include "igneous/scenes/SceneManager.hpp"

namespace Engine
{

    struct EngineInitSettings
    {
        int width = 640;
        int height = 360;
        std::string title = "client";
    };

    struct EngineInitState
    {
        bool sdl = false;
        bool mix = false;
        bool enet = false;
        bool input = false;
        bool sceneManager = false;
    };

    class Engine
    {
      private:

        static inline bool running = true;

        static inline EngineInitState initState{};

        static inline EngineInitSettings initSettings{};

      private:

        void HandleEvents();

        bool InitSDL();

        bool InitENet();

        void Init();

        void Update();

        void Clean() const;

      public:

        template<class T>
        requires(std::is_base_of_v<Scene, T>)
        int Run(const std::string& initialSceneName, const EngineInitSettings& settings = {});

        static void Quit();

#if defined(IGNEOUS_BUILD_TESTS)
        static void ResetForTests();
#endif
    };

#if defined(IGNEOUS_BUILD_TESTS)
    namespace TestHooks
    {
        inline bool forceInitSDLFailure = false;

        inline void Reset()
        {
            forceInitSDLFailure = false;
        }
    }
#endif

    template<class T>
    requires(std::is_base_of_v<Scene, T>)
    int Engine::Run(const std::string& initialSceneName, const EngineInitSettings& settings)
    {
        initSettings = settings;
        Init();
        if (!running)
        {
            Clean();
            return 1;
        }

        SceneManager::GetSceneRoot()->AddScene<T>(initialSceneName, "", true, false);
        Update();
        Clean();
        return 0;
    }
}
