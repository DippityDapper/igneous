#pragma once

#include <memory>
#include <unordered_map>
#include <queue>

namespace Engine
{
    class Scene;

    /// A utility class that manages scenes.
    class Scenes
    {
    private:
        inline static uint32_t nextSceneId = 1;

        /// A map of all created scenes.
        inline static std::unordered_map<uint32_t, Scene*> scenes{};

        /// A helper map to find a scene given a name.
        inline static std::unordered_map<std::string, uint32_t> sceneNameLookup{};

        /// A helper map that finds the name of a scene from its id.
        inline static std::unordered_map<uint32_t, std::string> sceneIdToName{};

    public:
        /// A list of the processing scenes.
        inline static std::unordered_map<uint32_t, Scene*> loadedScenes{};

        /// A queue of scenes ready to be loaded.
        inline static std::queue<Scene*> queuedLoadScenes{};

        /// A queue of scenes ready to be unloaded.
        inline static std::queue<Scene*> queuedUnloadScenes{};

        /// A queue of scenes ready to be deleted.
        inline static std::queue<Scene*> queueRemoveScenes{};

    public:
        Scenes() = delete;
        Scenes(const Scenes&) = delete;

        static void RunLoadQueue();
        static void RunUnloadQueue();
        static void RunRemoveQueue();
        static void Clean();

        /// Creates a scene.
        static Scene* CreateScene(Scene* scene, const std::string& name);

        /// Retrieves a scene given its name.
        static Scene* GetScene(const std::string& name);

        /// Removes a scene.
        static bool RemoveScene(const std::string& name);

        /// Removes all scenes from memory. Excludes singleton scenes.
        static bool RemoveAllScenes();

        /// Add a scene to the processing scenes queue given its name.
        static bool LoadScene(const std::string& name, bool unloadAll=true);

        /// Add a scene to the processing scenes queue.
        static bool LoadScene(Scene* scene, bool unloadAll=true);

        /// Unloads a scene from processing.
        static bool UnloadScene(const std::string& name);

        static void UnloadAllScenes();

        static bool SceneExists(const std::string& name);
    };
}
