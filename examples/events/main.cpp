#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Event.hpp"
#include "igneous/input/Input.hpp"

#include "imgui.h"
#include <SDL3/SDL_keycode.h>

class EventsScene : public Engine::Scene
{
  public:
    Engine::Event<void, int> onPing;
    Engine::Event<void, int>::Connection connection;
    int lastValue = 0;
    int emitCount = 0;

    void OnCreated() override
    {
        connection = onPing.Connect([this](int value)
                                    {
                                        lastValue = value;
                                    });
    }

    void OnDestroyed() override
    {
        onPing.Disconnect(connection);
    }

    void Render() override
    {
        ImGui::Begin("Event");
        ImGui::Text("Subscribe / emit demo using Engine::Event.");
        if (ImGui::Button("Emit ping"))
        {
            emitCount++;
            onPing.Emit(emitCount);
        }
        ImGui::Text("Last received value: %d", lastValue);
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
    return engine.Run<EventsScene>("Events");
}
