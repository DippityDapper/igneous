#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <memory>

#include "igneous/Sprite.hpp"
#include "SDL3_mixer/SDL_mixer.h"

namespace Engine
{
    class AudioStream;
}

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
        static inline std::multimap<int, Sprite*> spritesByZIndex{};

        static inline std::unordered_map<int, std::multimap<int, Sprite*>::iterator> spriteIterators{};

        /// A map of all loaded and created textures. A unique id is used to index into the map.
        static inline std::unordered_map<int, std::weak_ptr<SDL_Texture>> textures{};

        /// A map that tracks loaded texture file paths.
        static inline std::unordered_map<std::string, int> texturePathLookup{};

        /// A utility map that converts a texture id to its file path.
        static inline std::unordered_map<int, std::string> textureIdToPath{};

        /// The scale mode to use when loading or creating textures scale, ie nearest, linear, or pixel art
        static inline SDL_ScaleMode scaleMode = SDL_SCALEMODE_LINEAR;

        static inline std::unordered_map<int, std::weak_ptr<AudioStream>> sounds{};

        static inline std::unordered_map<std::string, int> soundPathLookup{};

        static inline std::unordered_map<int, std::string> soundIdToPath{};

    public:
        static inline MIX_Mixer* mixer = nullptr;

        static inline uint8_t trackCount = 16;

        static inline std::unordered_map<MIX_Track*, bool> tracks{};

    public:
        static void Clean();

    public:
        ResourceLoader(const ResourceLoader& other) = delete;

        static bool RegisterSprite(Sprite* sprite);

        static bool UnregisterSprite(int spriteId);

        static bool UpdateSpriteZIndex(Sprite* sprite, int newZIndex);

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

        static void RenderSprites();

        /// Sets the scale mode of loaded textures.
        /// @param _scaleMode The SDL scale mode.
        static void SetScaleMode(SDL_ScaleMode _scaleMode);

        static std::shared_ptr<AudioStream> LoadSound(const std::string& filePath, SDL_PropertiesID properties);

        static MIX_Mixer* GetMixer();
    };
}
