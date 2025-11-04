#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "SDL3/SDL.h"

namespace Engine
{
    /// Used to pass into the shared pointer for proper cleanup of the texture.
    struct TextureDeleter
    {
        void operator()(SDL_Texture* texture) const noexcept
        {
            if (texture)
            {
                SDL_DestroyTexture(texture);
            }
        }
    };

    /// Utility class that loads and creates textures.
    ///
    /// Unused textures are automatically cleaned periodically.
    class ResourceLoader
    {
    private:
        /// A map of all loaded and created textures. A unique id is used to index into the map.
        static std::unordered_map<int, std::weak_ptr<SDL_Texture>> textures;

        /// A map that tracks loaded texture file paths.
        static std::unordered_map<std::string, int> textureMap;

        /// A utility map that converts a texture id to its file path.
        static std::unordered_map<int, std::string> idToPath;

        /// The scale mode to use when loading or creating textures scale, ie nearest, linear, or pixel art
        static SDL_ScaleMode scaleMode;

    public:
        ResourceLoader(const ResourceLoader& other) = delete;

        /// Loads a texture given a file path.
        /// If the file path is found in the texture map, that texture will be returned.
        /// @param texturePath The file path of the texture.
        /// @returns a shared pointer to the SDL texture.
        static std::shared_ptr<SDL_Texture> LoadTexture(const std::string& filePath);

        /// Creates a texture.
        /// @param format The pixel format for the texture.
        /// @param access The texture access for the texture.
        /// @param w The width of the texture.
        /// @param h The height of the texture.
        /// @returns a shared pointer to the SDL texture.
        static std::shared_ptr<SDL_Texture> CreateTexture(SDL_PixelFormat format, SDL_TextureAccess access, int w, int h);

        /// Cleans unused textures.
        /// @param maxPerCall The max amount of textures that are cleaned per call.
        /// @note This is called within the Engine update loop.
        static void CleanExpired(size_t maxPerCall = 10);

        /// Sets the scale mode of loaded textures.
        /// @param _scaleMode The SDL scale mode.
        static void SetScaleMode(SDL_ScaleMode _scaleMode);
    };
}
