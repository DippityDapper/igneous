#pragma once

#include <memory>

#include "igneous/scenes/SceneRoot.hpp"

namespace Engine
{
    /**
     * @class SceneManager
     * @brief Manages the scene hierarchy root and scene lifecycle.
     *
     * The SceneManager provides a centralized interface for managing the scene
     * hierarchy through a single SceneRoot instance. The SceneRoot acts as the
     * top-level container for all active scenes in the engine.
     *
     * This class enforces a single-root constraint, ensuring only one SceneRoot
     * exists at a time. All scene management operations (adding, removing, updating
     * scenes) are performed through the SceneRoot.
     *
     * @note This is a static utility class and should not be instantiated.
     * @note Only one SceneRoot can exist at a time.
     * @note The SceneManager is initialized by the Engine during startup.
     *
     * @see SceneRoot
     * @see Scene
     */
    class SceneManager
    {
      private:
        /**
         * @brief The static SceneRoot instance.
         *
         * Manages the entire scene hierarchy. Created during Init() and
         * destroyed during RemoveSceneRoot().
         *
         * Initialized to nullptr before Init() is called.
         */
        static inline std::unique_ptr<SceneRoot> sceneRoot = nullptr;

      public:
        /**
         * @brief Initializes the SceneManager by creating a new SceneRoot.
         *
         * Creates a new SceneRoot instance that will serve as the top-level
         * container for all scenes in the engine.
         *
         * @return true if initialization succeeded, false otherwise.
         *
         * @note Called automatically by the Engine during initialization.
         * @note Should only be called once during engine startup.
         * @note Currently always returns true.
         */
        static bool Init();

        /**
         * @brief Sets a custom SceneRoot instance.
         *
         * Replaces the current SceneRoot with a provided one. This method enforces
         * the single-root constraint and will fail if:
         * - The provided root is null
         * - A SceneRoot already exists (must call RemoveSceneRoot() first)
         *
         * If successful, calls Init() on the new SceneRoot after taking ownership.
         *
         * @param root The new SceneRoot instance to set. Ownership is transferred
         *             to the SceneManager.
         *
         * @note Logs an error and returns without action if validation fails.
         * @note The provided root must not be null.
         * @note Remove the existing root before setting a new one.
         */
        static void SetSceneRoot(std::unique_ptr<SceneRoot> root);

        /**
         * @brief Removes and cleans up the current SceneRoot.
         *
         * If a SceneRoot exists, calls Clean() on it to properly cleanup all
         * child scenes and resources, then destroys the SceneRoot instance.
         *
         * @note Safe to call even if no SceneRoot exists.
         * @note Cleans all scenes in the hierarchy before destruction.
         * @note Required before setting a new SceneRoot via SetSceneRoot().
         */
        static void RemoveSceneRoot();

        /**
         * @brief Gets a pointer to the current SceneRoot.
         *
         * Provides access to the SceneRoot for scene management operations
         * such as adding scenes, updating the scene hierarchy, and rendering.
         *
         * @return Pointer to the SceneRoot, or nullptr if not initialized.
         *
         * @note Used extensively by the Engine for Update(), Render(), and
         *       event handling operations.
         * @note Check for nullptr before use, especially during initialization
         *       or shutdown.
         */
        static SceneRoot* GetSceneRoot();
    };
}