#include "igneous/scenes/SceneRoot.hpp"

#include <ranges>

#include "SDL3/SDL_log.h"

namespace Engine
{
    void SceneRoot::Init()
    {
    }

    void SceneRoot::Update(float delta)
    {
        for (const auto& scene: scenes | std::views::values)
        {
            if (scene->IsActive())
                scene->UpdateInternal(delta);
        }
    }

    void SceneRoot::UI(InputLayer& layer)
    {
        for (const auto& scene: scenes | std::views::values)
        {
            if (scene->IsActive())
                scene->UIInternal(layer);
        }
    }

    void SceneRoot::Render()
    {
        for (const auto& scene: scenes | std::views::values)
        {
            if (scene->IsActive())
                scene->RenderInternal();
        }
    }

    void SceneRoot::HandleEvents(InputLayer& layer)
    {
        for (const auto& scene: scenes | std::views::values)
        {
            if (scene->IsActive())
            {
                scene->HandleEventsInternal(layer);
            }
        }
    }

    void SceneRoot::Clean()
    {
        for (const auto& scene: scenes | std::views::values)
            scene->CleanInternal();
        scenes.clear();
    }

    void SceneRoot::RemoveScene(const std::string& name)
    {
        if (!scenes.contains(name))
        {
            SDL_Log("Scene %s does not exist", name.c_str());
            return;
        }
        Scene* scene = scenes[name].get();
        scene->CleanInternal();
        scenes.erase(name);
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
