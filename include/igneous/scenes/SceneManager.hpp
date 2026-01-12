#pragma once

#include <memory>

#include "igneous/scenes/SceneRoot.hpp"

namespace Engine
{
    /// A utility class that manages scenes.
    class SceneManager
    {
    private:
        static inline std::unique_ptr<SceneRoot> sceneRoot = nullptr;

    public:
        static bool Init();

        static void SetSceneRoot(std::unique_ptr<SceneRoot> root);
        static void RemoveSceneRoot();
        static SceneRoot* GetSceneRoot();
    };
}
