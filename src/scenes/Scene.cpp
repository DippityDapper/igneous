#include "igneous/scenes/Scene.hpp"

namespace Engine
{
    Scene::~Scene()
    {
        OnDestroyedInternal();
    }

    void Scene::OnCreatedInternal()
    {
        OnCreated();
    }

    void Scene::UpdateInternal(double delta)
    {
        Update(delta);
    }

    void Scene::RenderInternal()
    {
        Render();
    }

    void Scene::HandleInputsInternal(InputLayer& layer)
    {
        HandleInputs(layer);
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

    void Scene::OnDestroyedInternal()
    {
        OnDestroyed();
    }
}
