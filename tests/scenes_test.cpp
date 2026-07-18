#include <catch2/catch_test_macros.hpp>

#include "igneous/input/InputLayer.hpp"
#include "igneous/scenes/Scene.hpp"
#include "igneous/scenes/SceneManager.hpp"
#include "igneous/scenes/SceneRoot.hpp"

class TestScene : public Engine::Scene
{
  public:
    int updateCount = 0;

    void Update(double delta) override
    {
        (void) delta;
        ++updateCount;
    }
};

TEST_CASE("SceneRoot add load unload scenes", "[scenes][sceneroot]")
{
    Engine::SceneRoot root;

    TestScene* gameplay = root.AddScene<TestScene>("gameplay", "core", true, false);
    TestScene* menu = root.AddScene<TestScene>("menu", "ui", false, false);

    REQUIRE(gameplay != nullptr);
    REQUIRE(menu != nullptr);
    REQUIRE(root.SceneExists("gameplay"));
    REQUIRE(gameplay->IsActive());
    REQUIRE_FALSE(menu->IsActive());

    REQUIRE(root.LoadScene("menu"));
    REQUIRE(menu->IsActive());
    REQUIRE_FALSE(gameplay->IsActive());

    REQUIRE(root.UnloadScene("menu"));
    REQUIRE_FALSE(menu->IsActive());
}

TEST_CASE("SceneRoot deferred removal", "[scenes][sceneroot]")
{
    Engine::SceneRoot root;
    root.AddScene<TestScene>("temp", "", true, false);
    REQUIRE(root.SceneExists("temp"));

    root.RemoveScene("temp");
    REQUIRE(root.SceneExists("temp"));
    root.ProcessRemoveScenesQueue();
    REQUIRE_FALSE(root.SceneExists("temp"));
}

TEST_CASE("SceneManager owns scene root", "[scenes][scenemanager]")
{
    Engine::SceneManager::Init();
    REQUIRE(Engine::SceneManager::GetSceneRoot() != nullptr);

    Engine::SceneManager::RemoveSceneRoot();
    REQUIRE(Engine::SceneManager::GetSceneRoot() == nullptr);
}

TEST_CASE("Scene dispatches input by layer", "[scenes][scene]")
{
    struct LayerScene : Engine::Scene
    {
        std::string lastLayer;

        void HandleInputs(Engine::InputLayer& layer) override
        {
            lastLayer = layer.GetName();
        }
    };

    Engine::SceneRoot root;
    LayerScene* scene = root.AddScene<LayerScene>("main");

    Engine::InputLayer gameplay("gameplay", 0);
    scene->HandleInputsInternal(gameplay);
    REQUIRE(scene->lastLayer == "gameplay");
}

TEST_CASE("SceneRoot LoadScene unloads non-singleton scenes", "[scenes][sceneroot]")
{
    Engine::SceneRoot root;
    TestScene* gameplay = root.AddScene<TestScene>("gameplay", "core", true, false);
    TestScene* hud = root.AddScene<TestScene>("hud", "overlay", true, false);

    REQUIRE(gameplay->IsActive());
    REQUIRE(hud->IsActive());

    TestScene* menu = root.AddScene<TestScene>("menu", "ui", false, false);
    REQUIRE(root.LoadScene("menu", true));

    REQUIRE(menu->IsActive());
    REQUIRE_FALSE(gameplay->IsActive());
    REQUIRE_FALSE(hud->IsActive());
}

TEST_CASE("SceneRoot RemoveScenes removes by tag", "[scenes][sceneroot]")
{
    Engine::SceneRoot root;
    root.AddScene<TestScene>("gameplay", "core", true, false);
    root.AddScene<TestScene>("menu", "ui", false, false);
    root.AddScene<TestScene>("settings", "ui", false, false);

    root.RemoveScenes("ui");
    root.ProcessRemoveScenesQueue();

    REQUIRE(root.SceneExists("gameplay"));
    REQUIRE_FALSE(root.SceneExists("menu"));
    REQUIRE_FALSE(root.SceneExists("settings"));
}

TEST_CASE("SceneRoot singleton stays active on load and cannot unload", "[scenes][sceneroot]")
{
    Engine::SceneRoot root;
    TestScene* overlay = root.AddScene<TestScene>("overlay", "hud", true, true);
    TestScene* menu = root.AddScene<TestScene>("menu", "ui", false, false);

    REQUIRE(root.LoadScene("menu", true));
    REQUIRE(overlay->IsActive());
    REQUIRE(menu->IsActive());

    REQUIRE_FALSE(root.UnloadScene("overlay"));
    REQUIRE(overlay->IsActive());
}
