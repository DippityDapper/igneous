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

        /// Clears the scene root between unit tests.
        static void ResetForTests();

        static void SetSceneRoot(std::unique_ptr<SceneRoot> root);

        static void RemoveSceneRoot();

        static SceneRoot* GetSceneRoot();
    };
}
