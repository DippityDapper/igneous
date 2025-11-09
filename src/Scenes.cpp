#include "dapper2d/Scenes.hpp"

#include <random>

#include "SDL3/SDL_log.h"

#include "dapper2d/Scene.hpp"

namespace Engine
{
    std::unordered_map<int, Scene*> Scenes::loadedScenes{};
    std::queue<Scene*> Scenes::queuedScenes{};

    std::unordered_map<int, Scene*> Scenes::scenes{};
    std::unordered_map<std::string, int> Scenes::sceneNameLookup{};
    std::unordered_map<int, std::string> Scenes::idToName{};

    Scene* Scenes::CreateScene(Scene* scene, const std::string& name)
    {
        std::mt19937 gen(std::random_device{}());
        int sceneId = -1;
        do
        {
            std::uniform_int_distribution<> sceneIdDist(0, INT32_MAX);
            sceneId = sceneIdDist(gen);
        } while (scenes.contains(sceneId));

        scene->name = name;
        scene->id = sceneId;

        scenes.emplace(sceneId, scene);
        sceneNameLookup.emplace(name, sceneId);
        idToName.emplace(sceneId, name);

        if (loadedScenes.empty())
            queuedScenes.push(scene);

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
        scene->CleanInternal();
        scene->active = false;

        scenes.erase(id);
        sceneNameLookup.erase(name);
        idToName.erase(id);

        if (loadedScenes.contains(id))
            loadedScenes.erase(id);

        delete scene;

        return true;
    }

    bool Scenes::LoadScene(const std::string& name, bool unloadAll)
    {
        if (unloadAll)
            UnloadAll();

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

        if (loadedScenes.contains(id))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' already loaded.", name.c_str());
            return false;
        }

        Scene* scene = scenes[id];
        queuedScenes.push(scene);
        return true;
    }

    bool Scenes::LoadScene(Scene* scene, bool unloadAll)
    {
        if (unloadAll)
            UnloadAll();

        if (!sceneNameLookup.contains(scene->name))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' does not exist.", scene->name.c_str());
            return false;
        }

        const int id = scene->id;
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

        queuedScenes.push(scene);
        return true;
    }

    bool Scenes::UnloadScene(const std::string& name)
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

        if (!loadedScenes.contains(id))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Scene '%s' is not loaded.", name.c_str());
            return false;
        }

        Scene* scene = scenes[id];
        scene->CleanInternal();
        scene->active = false;
        loadedScenes.erase(id);

        return true;
    }

    void Scenes::UnloadAll()
    {
        int queueSize = queuedScenes.size();
        for (int i = 0; i < queueSize; i++)
        {
            Scene* queuedScene = queuedScenes.front();
            queuedScenes.pop();
            if (queuedScene->singleton)
            {
                queuedScenes.push(queuedScene);
                continue;
            }
            queuedScene->CleanInternal();
            queuedScene->active = false;
        }

        std::vector<int> scenesToRemove;
        scenesToRemove.reserve(loadedScenes.size());

        for (const auto &kvp : loadedScenes)
        {
            if (!kvp.second->singleton)
            {
                kvp.second->CleanInternal();
                kvp.second->active = false;
                scenesToRemove.push_back(kvp.first);
            }
        }

        for (const auto &key : scenesToRemove)
        {
            loadedScenes.erase(key);
        }
    }

    bool Scenes::SceneExists(const std::string& name)
    {
        if (!sceneNameLookup.contains(name))
            return false;

        const int id = sceneNameLookup[name];
        if (!scenes.contains(id))
            return false;

        return true;
    }

    void Scenes::LoadQueue()
    {
        int queueSize = queuedScenes.size();
        for (int i = 0; i < queueSize; i++)
        {
            Scene* scene = queuedScenes.front();
            scene->active = true;
            scene->InitInternal();
            queuedScenes.pop();
            loadedScenes.emplace(scene->id, scene);
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
        idToName.clear();
    }
}
