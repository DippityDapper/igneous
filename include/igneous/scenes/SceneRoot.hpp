// Doc: docs/classes/SceneRoot.md
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "SDL3/SDL_log.h"
#include "igneous/scenes/Scene.hpp"

#include <functional>
#include <queue>

namespace Engine
{

    class SceneRoot
    {
      private:

        std::unordered_map<std::string, std::unique_ptr<Scene>> scenes{};

        std::queue<std::string> scenesToRemove{};

      public:

        void ProcessRemoveScenesQueue();

        void Update(double delta);

        void Render();

        void HandleEvents(InputLayer& layer);

        void Clean();

        template<class T>
        requires(std::is_base_of_v<Scene, T>)
        T* AddScene(const std::string& name, const std::string& tag = "", bool active = true, bool singleton = false)
        {
            if (scenes.contains(name))
            {
                SDL_Log("Scene %s already exists.", name.c_str());
                return static_cast<T*>(scenes[name].get());
            }

            std::unique_ptr<T> scene = std::make_unique<T>();
            scene->root = this;
            scene->name = name;
            scene->tag = tag;
            scene->singleton = singleton;

            scene->OnCreatedInternal();
            if (scene->singleton)
                scene->SetActive(true);
            else
                scene->SetActive(active);

            scenes.emplace(name, std::move(scene));

            return static_cast<T*>(scenes[name].get());
        }

        void RemoveScene(const std::string& name);

        void RemoveScenes(const std::string& tag);

        bool LoadScene(const std::string& name, bool unloadAll = true);

        bool UnloadScene(const std::string& name);

        void UnloadAllScenes();

        Scene* GetScene(const std::string& name);

        bool SceneExists(const std::string& name);
    };
}
