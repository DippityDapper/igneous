// Doc: docs/classes/AnimationFrame.md
#pragma once
#include "SDL3/SDL_render.h"

#include <memory>

namespace Engine
{

    struct AnimationFrame
    {

        std::shared_ptr<SDL_Texture> texture = nullptr;

        int atlasW = 0;

        int atlasH = 0;

        int atlasX = 0;

        int atlasY = 0;
    };
}
