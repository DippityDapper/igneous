#include "igneous/engine/Database.hpp"
#include "igneous/engine/Engine.hpp"
#include "igneous/input/Input.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

#include <memory>

class DatabaseScene : public Engine::Scene
{
  public:
    std::unique_ptr<SQLite::Database> db;
    int clicks = 0;

    void OnCreated() override
    {
        db = Engine::Database::Open("assets/scores.db");
        if (!db)
            return;

        Engine::Database::Execute(db.get(), "CREATE TABLE IF NOT EXISTS stats (key TEXT PRIMARY KEY, value INTEGER);");

        auto rows = Engine::Database::Query(db.get(), "SELECT value FROM stats WHERE key = 'clicks';");
        if (!rows.empty())
            clicks = std::stoi(rows.front().at("value"));
    }

    void Render() override
    {
        ImGui::Begin("Database");
        ImGui::Text("Clicks stored in SQLite: %d", clicks);
        if (ImGui::Button("Increment and save") && db)
        {
            clicks++;
            Engine::Database::Execute(
                    db.get(),
                    "INSERT INTO stats(key, value) VALUES('clicks', " + std::to_string(clicks) +
                            ") ON CONFLICT(key) DO UPDATE SET value = " + std::to_string(clicks) + ";");
        }
        ImGui::End();
    }

    void HandleInputs(Engine::InputLayer& layer) override
    {
        (void) layer;
        if (Engine::Input::IsKeyJustPressed(SDLK_ESCAPE))
            Engine::Engine::Quit();
    }
};

int main()
{
    Engine::Engine engine;
    return engine.Run<DatabaseScene>("Database");
}
