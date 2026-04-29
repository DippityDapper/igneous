#pragma once
#include "SDL3/SDL_render.h"

#include <memory>

namespace Engine
{
    struct AnimationFrame
    {
        /**
         * @brief Shared pointer to the SDL texture.
         *
         * Multiple animation frames can share the same texture. The texture is kept alive
         * as long as at least one sprite or other shared_ptr references it.
         * The ResourceManager caches textures and handles cleanup.
         *
         * @note Lifetime is managed by the ResourceManager.
         */
        std::shared_ptr<SDL_Texture> texture = nullptr;

        /**
         * @brief The width to sample from the texture atlas in pixels.
         *
         * Defines the width of the region to extract from the texture.
         * For non-atlas textures, this equals the full texture width.
         * For atlas textures, this is the width of a single sprite cell.
         */
        int atlasW = 0;

        /**
         * @brief The height to sample from the texture atlas in pixels.
         *
         * Defines the height of the region to extract from the texture.
         * For non-atlas textures, this equals the full texture height.
         * For atlas textures, this is the height of a single sprite cell.
         */
        int atlasH = 0;

        /**
         * @brief The X grid coordinate to sample from the texture atlas.
         *
         * Used with atlasW to calculate the pixel offset: pixelX = atlasX * atlasW.
         * For non-atlas textures, this is typically 0.
         *
         * Example: For a sprite sheet with 32px cells, atlasX=2 samples at pixel 64.
         */
        int atlasX = 0;

        /**
         * @brief The Y grid coordinate to sample from the texture atlas.
         *
         * Used with atlasH to calculate the pixel offset: pixelY = atlasY * atlasH.
         * For non-atlas textures, this is typically 0.
         *
         * Example: For a sprite sheet with 32px cells, atlasY=3 samples at pixel 96.
         */
        int atlasY = 0;
    };
}