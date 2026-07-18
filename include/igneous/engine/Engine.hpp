#pragma once

#include "igneous/scenes/Scene.hpp"
#include "igneous/scenes/SceneManager.hpp"

namespace Engine
{

    class Engine
    {
      private:

        static inline bool running = true;

      private:

        void HandleEvents();

        bool InitSDL() const;

        bool InitENet() const;

        void Init();

        void Update();

        void Clean() const;

      public:

        template<class T>
        requires(std::is_base_of_v<Scene, T>)
        int Run(const std::string& initialSceneName);

        static void Quit();
    };

    template<class T>
    requires(std::is_base_of_v<Scene, T>)
    int Engine::Run(const std::string& initialSceneName)
    {
        Init();
        SceneManager::GetSceneRoot()->AddScene<T>(initialSceneName, "", true, false);
        Update();
        Clean();
        return 0;
    }
}
