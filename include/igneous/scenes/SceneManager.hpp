#pragma once

#include <memory>

#include "igneous/scenes/SceneRoot.hpp"

namespace Engine
{

    class SceneManager
    {
      private:

        static inline std::unique_ptr<SceneRoot> sceneRoot = nullptr;

      public:

        static bool Init();

        static void ResetForTests();

        static void SetSceneRoot(std::unique_ptr<SceneRoot> root);

        static void RemoveSceneRoot();

        static SceneRoot* GetSceneRoot();
    };
}
