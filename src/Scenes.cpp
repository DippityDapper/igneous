#include "igneous/Scenes.hpp"

#include "SDL3/SDL_log.h"

#include "igneous/Scene.hpp"

namespace Engine
{
    Scene* Scenes::CreateScene(Scene* scene, const std::string& name)
    {
        if (name.empty())
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene name not set.");
            return nullptr;
        }
        uint32_t sceneId = nextSceneId++;

        scene->name = name;
        scene->id = sceneId;

        scenes.emplace(sceneId, scene);
        sceneNameLookup.emplace(name, sceneId);
        sceneIdToName.emplace(sceneId, name);

        if (loadedScenes.empty())
            queuedLoadScenes.push(scene);

        return scene;
    }

    Scene* Scenes::GetScene(const std::string& name)
    {
        if (!sceneNameLookup.contains(name))
            return nullptr;

        const int id = sceneNameLookup[name];
        if (!scenes.contains(id))
            return nullptr;

        return scenes[id];
    }

    bool Scenes::RemoveScene(const std::string& name)
    {
        if (!sceneNameLookup.contains(name))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' does not exist.", name.c_str());
            return false;
        }

        const int id = sceneNameLookup[name];
        if (!scenes.contains(id))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' does not exist.", name.c_str());
            return false;
        }

        Scene* scene = scenes[id];
        queueRemoveScenes.push(scene);

        return true;
    }

    bool Scenes::RemoveAllScenes()
    {
        std::vector<Scene*> scenesToRemove;
        scenesToRemove.reserve(scenes.size());

        for (const auto &kvp : scenes)
        {
            if (!kvp.second->singleton)
                scenesToRemove.push_back(kvp.second);
        }

        for (const auto& scene : scenesToRemove)
            queueRemoveScenes.push(scene);

        return true;
    }

    bool Scenes::LoadScene(const std::string& name, bool unloadAll)
    {
        if (unloadAll)
            UnloadAllScenes();

        if (!sceneNameLookup.contains(name))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' does not exist.", name.c_str());
            return false;
        }

        const uint32_t id = sceneNameLookup[name];
        if (!scenes.contains(id))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' does not exist.", name.c_str());
            return false;
        }

        if (loadedScenes.contains(id))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' already loaded.", name.c_str());
            return false;
        }

        Scene* scene = scenes[id];
        queuedLoadScenes.push(scene);
        return true;
    }

    bool Scenes::LoadScene(Scene* scene, bool unloadAll)
    {
        if (unloadAll)
            UnloadAllScenes();

        if (!sceneNameLookup.contains(scene->name))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' does not exist.", scene->name.c_str());
            return false;
        }

        const uint32_t id = scene->id;
        if (!scenes.contains(id))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' does not exist.", scene->name.c_str());
            return false;
        }

        if (loadedScenes.contains(id))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' already loaded.", scene->name.c_str());
            return false;
        }

        queuedLoadScenes.push(scene);
        return true;
    }

    bool Scenes::UnloadScene(const std::string& name)
    {
        if (!sceneNameLookup.contains(name))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' does not exist.", name.c_str());
            return false;
        }

        const uint32_t id = sceneNameLookup[name];
        if (!scenes.contains(id))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' does not exist.", name.c_str());
            return false;
        }

        if (!loadedScenes.contains(id))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' is not loaded.", name.c_str());
            return false;
        }

        Scene* scene = scenes[id];
        queuedUnloadScenes.push(scene);

        return true;
    }

    void Scenes::UnloadAllScenes()
    {
        std::vector<Scene*> scenesToUnload;
        scenesToUnload.reserve(loadedScenes.size());

        for (const auto& kvp : loadedScenes)
        {
            if (!kvp.second->singleton)
                scenesToUnload.push_back(kvp.second);
        }

        for (const auto& scene : scenesToUnload)
            queuedUnloadScenes.push(scene);
    }

    bool Scenes::SceneExists(const std::string& name)
    {
        if (!sceneNameLookup.contains(name))
            return false;

        const uint32_t id = sceneNameLookup[name];
        if (!scenes.contains(id))
            return false;

        return true;
    }

    void Scenes::RunLoadQueue()
    {
        int queueSize = queuedLoadScenes.size();
        for (int i = 0; i < queueSize; i++)
        {
            Scene* scene = queuedLoadScenes.front();
            queuedLoadScenes.pop();

            scene->active = true;
            scene->InitInternal();
            loadedScenes.emplace(scene->id, scene);
        }
    }

    void Scenes::RunUnloadQueue()
    {
        int queueSize = queuedUnloadScenes.size();

        for (int i = 0; i < queueSize; i++)
        {
            Scene* scene = queuedUnloadScenes.front();
            queuedUnloadScenes.pop();

            scene->active = false;
            scene->CleanInternal();
            loadedScenes.erase(scene->id);
        }
    }

    void Scenes::RunRemoveQueue()
    {
        int queueSize = queueRemoveScenes.size();
        for (int i = 0; i < queueSize; i++)
        {
            Scene* scene = queueRemoveScenes.front();
            queueRemoveScenes.pop();

            if (scene->active)
            {
                queueRemoveScenes.push(scene);
                scene->active = false;
                continue;
            }
            scene->active = false;
            scene->CleanInternal();

            if (scenes.contains(scene->id))
                scenes.erase(scene->id);
            if (sceneNameLookup.contains(scene->name))
                sceneNameLookup.erase(scene->name);
            if (sceneIdToName.contains(scene->id))
                sceneIdToName.erase(scene->id);
            if (loadedScenes.contains(scene->id))
                loadedScenes.erase(scene->id);

            delete scene;
        }
    }

    void Scenes::Clean()
    {
        if (scenes.empty())
            return;

        for (const auto& kvp : scenes)
        {
            kvp.second->CleanInternal();
            delete kvp.second;
        }
        scenes.clear();
        sceneNameLookup.clear();
        sceneIdToName.clear();
    }
}
