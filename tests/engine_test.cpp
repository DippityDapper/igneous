#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "igneous/engine/CFGParser.hpp"
#include "igneous/engine/Camera.hpp"
#include "igneous/engine/Database.hpp"
#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Event.hpp"
#include "igneous/engine/PerlinNoise.hpp"
#include "igneous/engine/ThreadPool.hpp"
#include "igneous/engine/ThreadSafeQueue.hpp"
#include "igneous/engine/Time.hpp"
#include "igneous/engine/Vec2.hpp"
#include "igneous/engine/Vec3.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/rendering/Window.hpp"

#include "igneous/scenes/Scene.hpp"

#include <SDL3/SDL.h>
#include <filesystem>
#include <fstream>
#include <future>
#include <climits>

using Catch::Matchers::WithinAbs;

class EngineInitTestScene : public Engine::Scene
{
  public:
    void Update(double delta) override
    {
        (void) delta;
        Engine::Engine::Quit();
    }
};

class EngineWindowSettingsTestScene : public Engine::Scene
{
  public:
    static inline int observedWidth = 0;
    static inline int observedHeight = 0;
    static inline std::string observedTitle;

    void Update(double delta) override
    {
        (void) delta;
        observedWidth = Engine::Window::viewport.x;
        observedHeight = Engine::Window::viewport.y;
        if (Engine::Window::GetWindow())
            observedTitle = SDL_GetWindowTitle(Engine::Window::GetWindow());
        Engine::Engine::Quit();
    }
};

TEST_CASE("Vec2 arithmetic and helpers", "[engine][vec2]")
{
    Engine::Vec2<float> a{3.0f, 4.0f};
    Engine::Vec2<float> b{1.0f, 2.0f};

    REQUIRE(a + b == Engine::Vec2<float>{4.0f, 6.0f});
    REQUIRE(a - b == Engine::Vec2<float>{2.0f, 2.0f});
    REQUIRE_THAT(a.Magnitude(), WithinAbs(5.0f, 0.001f));
    REQUIRE_THAT(a.DistanceTo(b), WithinAbs(std::sqrt(8.0f), 0.001f));

    Engine::Vec2<float> normalized = a.Normalized();
    REQUIRE_THAT(normalized.x, WithinAbs(0.6f, 0.001f));
    REQUIRE_THAT(normalized.y, WithinAbs(0.8f, 0.001f));

    Engine::Vec2<float> fractional{0.1f, 0.2f};
    REQUIRE_THAT(fractional.Magnitude(), WithinAbs(std::sqrt(0.05f), 0.001f));
    REQUIRE_THAT(fractional.Dot(Engine::Vec2<float>{0.3f, 0.4f}), WithinAbs(0.11f, 0.001f));
    REQUIRE(fractional != Engine::Vec2<float>{0.1f, 0.3f});
}

TEST_CASE("Vec3 arithmetic", "[engine][vec3]")
{
    Engine::Vec3<int> a{1, 2, 3};
    Engine::Vec3<int> b{4, 5, 6};

    REQUIRE(a + b == Engine::Vec3<int>{5, 7, 9});
    REQUIRE(a * 2 == Engine::Vec3<int>{2, 4, 6});

    Engine::Vec3<float> fractional{0.1f, 0.2f, 0.2f};
    REQUIRE_THAT(fractional.Magnitude(), WithinAbs(std::sqrt(0.09f), 0.001f));
    REQUIRE_THAT(fractional.Dot(Engine::Vec3<float>{0.3f, 0.4f, 0.5f}), WithinAbs(0.21f, 0.001f));

    Engine::Vec3<float> normalized = fractional.Normalized();
    REQUIRE_THAT(normalized.Magnitude(), WithinAbs(1.0f, 0.001f));
}

TEST_CASE("Engine Run returns non-zero on init failure", "[engine][init]")
{
    Engine::Engine::ResetForTests();
    Engine::TestHooks::forceInitSDLFailure = true;

    Engine::Engine engine;
    const int exitCode = engine.Run<EngineInitTestScene>("main");

    REQUIRE(exitCode != 0);
    REQUIRE(Engine::SceneManager::GetSceneRoot() == nullptr);

    Engine::TestHooks::Reset();
    Engine::Engine::ResetForTests();
}

TEST_CASE("Engine init settings configure window", "[engine][window]")
{
    ::EngineWindowSettingsTestScene::observedWidth = 0;
    ::EngineWindowSettingsTestScene::observedHeight = 0;
    ::EngineWindowSettingsTestScene::observedTitle.clear();

    Engine::EngineInitSettings settings;
    settings.width = 1280;
    settings.height = 720;
    settings.title = "Custom Title";

    Engine::Engine engine;
    const int exitCode = engine.Run<EngineWindowSettingsTestScene>("main", settings);

    REQUIRE(exitCode == 0);
    REQUIRE(::EngineWindowSettingsTestScene::observedWidth == 1280);
    REQUIRE(::EngineWindowSettingsTestScene::observedHeight == 720);
    REQUIRE(::EngineWindowSettingsTestScene::observedTitle == "Custom Title");
    REQUIRE(Engine::SceneManager::GetSceneRoot() == nullptr);
}

TEST_CASE("Engine Clean restores static manager baseline", "[engine][lifecycle]")
{
    Engine::Engine engine;
    const int exitCode = engine.Run<EngineInitTestScene>("main");

    REQUIRE(exitCode == 0);
    REQUIRE(Engine::SceneManager::GetSceneRoot() == nullptr);
    REQUIRE(Engine::Camera::main == nullptr);

    const auto layers = Engine::Input::GetInputLayers();
    REQUIRE(layers.size() == 1);
    REQUIRE(layers.front()->GetName() == "_default");
}

TEST_CASE("Event connect emit disconnect", "[engine][event]")
{
    Engine::Event<void, int> event;
    int total = 0;

    auto connection = event.Connect([&total](int value) { total += value; });
    event.Emit(2);
    event.Emit(3);
    REQUIRE(total == 5);

    event.Disconnect(connection);
    event.Emit(10);
    REQUIRE(total == 5);
}

TEST_CASE("CFGParser load read write", "[engine][cfgparser]")
{
    const std::filesystem::path path = std::filesystem::temp_directory_path() / "igneous_cfg_test.cfg";
    {
        std::ofstream file(path);
        file << "volume=0.75\n";
        file << "fullscreen=true\n";
        file << "name=Igneous\n";
    }

    Engine::CFGParser::LoadConfig(path.string(), "test");
    REQUIRE(Engine::CFGParser::GetString("test", "name") == "Igneous");
    REQUIRE(Engine::CFGParser::GetBool("test", "fullscreen"));
    REQUIRE_THAT(std::stof(Engine::CFGParser::GetString("test", "volume")), WithinAbs(0.75f, 0.001f));

    Engine::CFGParser::WriteInt("test", "lives", 3);
    Engine::CFGParser::SaveConfig(path.string(), "test");
    Engine::CFGParser::LoadConfig(path.string(), "test");
    REQUIRE(Engine::CFGParser::GetInt("test", "lives") == 3);

    std::filesystem::remove(path);
}

TEST_CASE("CFGParser uint16 read write and clamp", "[engine][cfgparser]")
{
    const std::filesystem::path path = std::filesystem::temp_directory_path() / "igneous_cfg_uint16_test.cfg";
    Engine::CFGParser::LoadConfig(path.string(), "uint16");
    Engine::CFGParser::WriteUInt16("uint16", "port", 7777);
    Engine::CFGParser::SaveConfig(path.string(), "uint16");
    Engine::CFGParser::LoadConfig(path.string(), "uint16");
    REQUIRE(Engine::CFGParser::GetUInt16("uint16", "port") == 7777);

    {
        std::ofstream file(path);
        file << "port=7777\n";
        file << "overflow=70000\n";
    }
    Engine::CFGParser::LoadConfig(path.string(), "uint16");
    REQUIRE(Engine::CFGParser::GetUInt16("uint16", "overflow") == UINT16_MAX);

    std::filesystem::remove(path);
}

TEST_CASE("Database open execute query", "[engine][database]")
{
    const std::filesystem::path path = std::filesystem::temp_directory_path() / "igneous_db_test.sqlite";
    std::filesystem::remove(path);

    auto db = Engine::Database::Open(path.string());
    REQUIRE(db != nullptr);
    REQUIRE(Engine::Database::Execute(db.get(), "CREATE TABLE items (id INTEGER PRIMARY KEY, name TEXT);"));
    REQUIRE(Engine::Database::Execute(db.get(), "INSERT INTO items (name) VALUES ('sword');"));

    auto rows = Engine::Database::Query(db.get(), "SELECT name FROM items;");
    REQUIRE(rows.size() == 1);
    REQUIRE(rows.front().at("name") == "sword");

    std::filesystem::remove(path);
}

TEST_CASE("Perlin noise is deterministic for seed", "[engine][perlin]")
{
    Engine::Perlin a(42);
    Engine::Perlin b(42);
    Engine::Perlin c(99);

    REQUIRE_THAT(a.Noise2(1.25f, 2.5f), WithinAbs(b.Noise2(1.25f, 2.5f), 0.0001f));
    REQUIRE(a.Noise2(1.25f, 2.5f) != c.Noise2(1.25f, 2.5f));
    REQUIRE_THAT(a.Fbm2_01(10.0f, 20.0f), WithinAbs(0.0f, 1.0f));
}

TEST_CASE("ThreadPool runs enqueued work", "[engine][threadpool]")
{
    Engine::ThreadPool pool(2);
    std::promise<int> promise;
    auto future = pool.Enqueue([&promise]() {
        promise.set_value(21);
        return 21;
    });

    REQUIRE(future.get() == 21);
    REQUIRE(promise.get_future().get() == 21);
}

TEST_CASE("ThreadSafeQueue push pop", "[engine][threadsafqueue]")
{
    Engine::ThreadSafeQueue<int> queue;
    REQUIRE(queue.Empty());

    queue.Push(7);
    auto value = queue.Pop();
    REQUIRE(value.has_value());
    REQUIRE(*value == 7);
    REQUIRE(queue.Empty());
}

TEST_CASE("Time static fields are writable", "[engine][time]")
{
    Engine::Time::lastTick = 100;
    Engine::Time::currentTick = 120;
    Engine::Time::deltaTime = 0.02;

    REQUIRE(Engine::Time::currentTick - Engine::Time::lastTick == 20);
    REQUIRE_THAT(Engine::Time::deltaTime, WithinAbs(0.02, 0.0001));
}

TEST_CASE("Camera clamp respects bounds", "[engine][camera]")
{
    Engine::Window::viewport = {640, 360};

    Engine::Camera camera;
    camera.limitBounds = true;
    camera.limitLeft = 0;
    camera.limitRight = 1000;
    camera.limitTop = 0;
    camera.limitBottom = 1000;
    camera.zoom = 1.0f;

    Engine::Vec2<float> clamped = camera.ClampToBounds({-500.0f, 500.0f}, 1.0f);
    REQUIRE(clamped.x >= camera.limitLeft);
    REQUIRE(clamped.x <= camera.limitRight);
    REQUIRE(clamped.y >= camera.limitTop);
    REQUIRE(clamped.y <= camera.limitBottom);
}
