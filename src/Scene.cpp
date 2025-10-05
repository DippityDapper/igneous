#include "dapper2d/Scene.hpp"

namespace Engine
{

    void Scene::InitInternal()
    {
        Init();
    }

    void Scene::UpdateInternal(float delta)
    {
        Update(delta);
    }

    void Scene::RenderInternal()
    {
        Render();
    }

    void Scene::HandleEventsInternal(SDL_Event& event)
    {
        HandleEvents(event);
    }

    void Scene::CleanInternal()
    {
        Clean();
    }
}
