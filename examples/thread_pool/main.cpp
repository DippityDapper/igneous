#include "igneous/engine/Engine.hpp"
#include "igneous/engine/ThreadPool.hpp"
#include "igneous/input/Input.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

class ThreadPoolScene : public Engine::Scene
{
  public:
    Engine::ThreadPool pool{4};
    std::atomic<int> completed{0};
    std::atomic<int> running{0};

    void Render() override
    {
        ImGui::Begin("ThreadPool");
        ImGui::Text("Completed tasks: %d", completed.load());
        ImGui::Text("Running tasks: %d", running.load());
        if (ImGui::Button("Enqueue 8 background tasks"))
        {
            for (int i = 0; i < 8; ++i)
            {
                running++;
                pool.Enqueue([this]()
                             {
                                 std::this_thread::sleep_for(std::chrono::milliseconds(250));
                                 completed++;
                                 running--;
                             });
            }
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
    return engine.Run<ThreadPoolScene>("ThreadPool");
}
