#include "igneous/scenes/SceneRoot.hpp"

#include <ranges>

#include "SDL3/SDL_log.h"

namespace Engine
{
    void SceneRoot::ProcessRemoveScenesQueue()
    {
        if (!scenesToRemove.empty())
        {
            for (int i = 0; i < scenesToRemove.size(); ++i)
            {
                std::string sceneName = scenesToRemove.front();
                scenesToRemove.pop();
                if (!scenes.contains(sceneName))
                    continue;

                Scene* scene = scenes[sceneName].get();
                scene->OnDestroyedInternal();
                scenes.erase(sceneName);
            }
        }
    }

    void SceneRoot::Update(double delta)
    {
        std::vector<Scene*> active;
        active.reserve(scenes.size());
        for (const auto& scene: scenes | std::views::values)
            if (scene->IsActive())
                active.push_back(scene.get());

        for (Scene* scene: active)
            scene->UpdateInternal(delta);
    }

    void SceneRoot::Render()
    {
        std::vector<Scene*> active;
        active.reserve(scenes.size());
        for (const auto& scene: scenes | std::views::values)
            if (scene->IsActive())
                active.push_back(scene.get());

        for (Scene* scene: active)
            scene->RenderInternal();
    }

    void SceneRoot::HandleEvents(InputLayer& layer)
    {
        std::vector<Scene*> active;
        active.reserve(scenes.size());
        for (const auto& scene: scenes | std::views::values)
            if (scene->IsActive())
                active.push_back(scene.get());

        for (Scene* scene: active)
            scene->HandleInputsInternal(layer);
    }

    void SceneRoot::Clean()
    {
        for (const auto& scene: scenes | std::views::values)
            scene->OnDestroyedInternal();
        scenes.clear();
    }

    void SceneRoot::RemoveScene(const std::string& name)
    {
        if (!scenes.contains(name))
        {
            SDL_Log("Scene %s does not exist", name.c_str());
            return;
        }
        scenesToRemove.emplace(name);
    }

    void SceneRoot::RemoveScenes(const std::string& tag)
    {
        for (const auto& scene: scenes | std::views::values)
        {
            if (scene->tag == tag)
                scenesToRemove.emplace(scene.get()->name);
        }
    }

    bool SceneRoot::LoadScene(const std::string& name, bool unloadAll)
    {
        if (!scenes.contains(name))
        {
            SDL_Log("Scene %s does not exist", name.c_str());
            return false;
        }

        if (unloadAll)
        {
            for (const auto& scene: scenes | std::views::values)
            {
                if (!scene->singleton)
                    scene->SetActive(false);
            }
        }

        Scene* scene = scenes[name].get();
        scene->SetActive(true);
        return true;
    }

    bool SceneRoot::UnloadScene(const std::string& name)
    {
        if (!scenes.contains(name))
        {
            SDL_Log("Scene %s does not exist", name.c_str());
            return false;
        }
        Scene* scene = scenes[name].get();
        if (scene->singleton)
        {
            SDL_Log("Cannot unload scene %s. Scene is a singleton.", name.c_str());
            return false;
        }
        scene->SetActive(false);
        return true;
    }

    void SceneRoot::UnloadAllScenes()
    {
        for (const auto& scene: scenes | std::views::values)
        {
            if (!scene->singleton)
                scene->SetActive(false);
        }
    }

    Scene* SceneRoot::GetScene(const std::string& name)
    {
        if (!scenes.contains(name))
        {
            SDL_Log("Scene %s does not exist", name.c_str());
            return nullptr;
        }
        return scenes[name].get();
    }

    bool SceneRoot::SceneExists(const std::string& name)
    {
        return scenes.contains(name);
    }
}
