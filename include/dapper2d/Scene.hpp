#pragma once

union SDL_Event;

namespace Engine
{
    class Scene
    {
    public:
        void InitInternal();
        void UpdateInternal(float delta);
        void RenderInternal();
        void CleanInternal();
        void HandleEventsInternal(SDL_Event& event);

    private:
        virtual void Init(){};
        virtual void Update(float delta){};
        virtual void Render(){};
        virtual void Clean(){};
        virtual void HandleEvents(SDL_Event& event){};
    };
}
