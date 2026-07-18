#pragma once

#include <string>

#include "igneous/input/InputLayer.hpp"

namespace Engine
{

    class SceneRoot;

    class Scene
    {
      public:

        SceneRoot* root = nullptr;

        std::string name{};

        std::string tag{};

        bool singleton = false;

      private:

        bool active = false;

      public:

        virtual ~Scene();

        void OnCreatedInternal();

        void UpdateInternal(double delta);

        void RenderInternal();

        void OnDestroyedInternal();

        void HandleInputsInternal(InputLayer& layer);

        void SetActive(bool value);

        bool IsActive();

      private:

        virtual void OnCreated()
        {
        }

        virtual void Update(double delta)
        {
        }

        virtual void Render()
        {
        }

        virtual void HandleInputs(InputLayer& layer)
        {
        }

        virtual void OnDestroyed()
        {
        }

        virtual void OnActiveChanged(bool value)
        {
        }
    };
}
