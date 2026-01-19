#include "igneous/scenes/SceneManager.hpp"

#include "SDL3/SDL_log.h"

namespace Engine
{
    bool SceneManager::Init()
    {
        sceneRoot = std::make_unique<SceneRoot>();
        return true;
    }

    void SceneManager::SetSceneRoot(std::unique_ptr<SceneRoot> root)
    {
        if (!root)
        {
            SDL_Log("Cannot set scene root. Scene root is null.");
            return;
        }
        if (sceneRoot)
        {
            SDL_Log("Only one scene root can exist. Remove the current scene root to set a new one.");
            return;
        }
        sceneRoot = std::move(root);
        sceneRoot->Init();
    }

    void SceneManager::RemoveSceneRoot()
    {
        if (sceneRoot)
            sceneRoot->Clean();
        sceneRoot = nullptr;
    }

    SceneRoot* SceneManager::GetSceneRoot()
    {
        return sceneRoot.get();
    }
}
