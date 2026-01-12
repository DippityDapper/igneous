#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "igneous/scenes/Scene.hpp"

namespace Engine
{
    class SceneRoot
    {
    private:
        std::unordered_map<std::string, std::unique_ptr<Scene>> scenes{};

    public:
        void Init();
        void Update(float delta);
        void UI(InputLayer& layer);
        void Render();
        void HandleEvents(InputLayer& layer);
        void Clean();

        template<class T> requires(std::is_base_of_v<Scene, T>)
        T* AddScene(const std::string& name, bool active=true, bool singleton=false);
        void RemoveScene(const std::string& name);

        bool LoadScene(const std::string& name, bool unloadAll=true);
        bool UnloadScene(const std::string& name);
        void UnloadAllScenes();

        Scene* GetScene(const std::string& name);
        bool SceneExists(const std::string& name);
    };

    template<class T> requires (std::is_base_of_v<Scene, T>)
    T* SceneRoot::AddScene(const std::string &name, bool active, bool singleton)
    {
        if (scenes.contains(name))
        {
            return nullptr;
        }

        std::unique_ptr<T> scene = std::make_unique<T>();
        scene->root = this;
        scene->name = name;
        scene->singleton = singleton;

        scene->InitInternal();
        if (scene->singleton)
            scene->SetActive(true);
        else
            scene->SetActive(active);

        scenes.emplace(name, std::move(scene));


        return reinterpret_cast<T*>(scenes[name].get());
    }
}
