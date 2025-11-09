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
        /// A map of all created scenes.
        static std::unordered_map<int, Scene*> scenes;

        /// A helper map to find a scene given a name.
        static std::unordered_map<std::string, int> sceneNameLookup;

        /// A helper map that finds the name of a scene from its id.
        static std::unordered_map<int, std::string> idToName;

    public:
        /// A list of the processing scenes.
        static std::unordered_map<int, Scene*> loadedScenes;

        /// A queue of scenes ready to be loaded.
        static std::queue<Scene*> queuedScenes;

    public:
        Scenes() = delete;
        Scenes(const Scenes&) = delete;

        static void LoadQueue();
        static void Clean();

        /// Creates a scene.
        static Scene* CreateScene(Scene* scene, const std::string& name);

        /// Retrieves a scene given its name.
        static Scene* GetScene(const std::string& name);

        /// Removes a scene.
        static bool RemoveScene(const std::string& name);

        /// Add a scene to the processing scenes queue given its name.
        static bool LoadScene(const std::string& name, bool unloadAll=true);

        /// Add a scene to the processing scenes queue.
        static bool LoadScene(Scene* scene, bool unloadAll=true);

        /// Unloads a scene from processing.
        static bool UnloadScene(const std::string& name);

        static void UnloadAll();

        static bool SceneExists(const std::string& name);
    };
}
