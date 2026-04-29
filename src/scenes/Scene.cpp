#include "igneous/scenes/Scene.hpp"

namespace Engine
{
    Scene::~Scene()
    {
        CleanInternal();
    }

    void Scene::InitInternal()
    {
        Init();
    }

    void Scene::UpdateInternal(double delta)
    {
        Update(delta);
    }

    void Scene::UIInternal(InputLayer& layer)
    {
        UI(layer);
    }

    void Scene::RenderInternal()
    {
        Render();
    }

    void Scene::HandleEventsInternal(InputLayer& layer)
    {
        HandleEvents(layer);
    }

    void Scene::SetActive(bool value)
    {
        if (value == active)
            return;

        active = value;
        OnActiveChanged(value);
    }

    bool Scene::IsActive()
    {
        return active;
    }

    void Scene::CleanInternal()
    {
        Clean();
    }
}
