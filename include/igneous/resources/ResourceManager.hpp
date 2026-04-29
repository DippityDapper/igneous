#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <memory>

#include "igneous/resources/Sprite.hpp"
#include "SDL3_mixer/SDL_mixer.h"

namespace Engine
{
    class AudioStream;
}

namespace Engine
{
    /**
     * @struct TextureDeleter
     * @brief Custom deleter for SDL_Texture shared_ptr.
     *
     * Ensures proper cleanup of SDL textures when the last shared_ptr
     * reference is released. Used automatically by std::shared_ptr.
     */
    struct TextureDeleter
    {
        /**
         * @brief Deletes an SDL_Texture using SDL_DestroyTexture.
         *
         * @param texture The texture to destroy.
         */
        void operator()(SDL_Texture* texture) const noexcept
        {
            if (texture)
            {
                SDL_DestroyTexture(texture);
            }
        }
    };

    /**
     * @class ResourceManager
     * @brief Centralized resource loading, caching, and management system.
     *
     * The ResourceManager handles:
     * - **Texture loading and caching**: Loads textures from disk and caches them
     *   by file path to avoid duplicate loads
     * - **Texture creation**: Creates programmatic textures for render targets
     * - **Sprite registration and rendering**: Manages sprite Z-ordering and
     *   automatic rendering
     * - **Audio loading and playback**: Loads audio files and manages the track pool
     * - **Resource cleanup**: Automatically cleans up unused resources
     *
     * ## Texture Management
     *
     * Textures are stored as weak_ptr, allowing automatic cleanup when no sprites
     * reference them. The ResourceManager caches textures by file path, so loading
     * the same file multiple times returns the same texture instance.
     *
     * ## Sprite Rendering
     *
     * Sprites are automatically registered during construction and rendered each
     * frame sorted by Z-index. Higher Z-index values render on top.
     *
     * ## Audio System
     *
     * The audio system uses a fixed pool of tracks (default 16). AudioStreams share
     * these tracks for playback. If all tracks are busy, the oldest is stopped
     * to make room.
     *
     * @note This is a static utility class and should not be instantiated.
     * @note Resources are cleaned periodically by the Engine via CleanExpired().
     *
     * @see Sprite
     * @see AudioStream
     * @see TextureDeleter
     */
    class ResourceManager
    {
      private:
        /**
         * @brief Multimap of sprites sorted by Z-index.
         *
         * Used to render sprites in the correct order (lowest Z first).
         * Multiple sprites can share the same Z-index.
         */
        static inline std::multimap<int, Sprite*> spritesByZIndex{};

        /**
         * @brief Lookup map from sprite ID to its position in spritesByZIndex.
         *
         * Allows O(1) sprite removal and Z-index updates.
         */
        static inline std::unordered_map<int, std::multimap<int, Sprite*>::iterator> spriteIterators{};

        /**
         * @brief Map of all loaded textures by unique ID.
         *
         * Stored as weak_ptr to allow automatic cleanup when no sprites
         * reference the texture.
         */
        static inline std::unordered_map<int, std::weak_ptr<SDL_Texture>> textures{};

        /**
         * @brief Lookup map from file path to texture ID.
         *
         * Enables texture caching by file path. When loading a texture,
         * this map is checked first to return existing textures.
         */
        static inline std::unordered_map<std::string, int> texturePathLookup{};

        /**
         * @brief Reverse lookup map from texture ID to file path.
         *
         * Used during cleanup to remove entries from texturePathLookup
         * when textures expire.
         */
        static inline std::unordered_map<int, std::string> textureIdToPath{};

        /**
         * @brief The scale mode to apply to newly loaded/created textures.
         *
         * Common values:
         * - SDL_SCALEMODE_NEAREST: Pixel-perfect scaling (sharp pixels)
         * - SDL_SCALEMODE_LINEAR: Smooth scaling (default)
         * - SDL_SCALEMODE_BEST: Highest quality scaling
         *
         * Can be changed via SetScaleMode().
         */
        static inline SDL_ScaleMode scaleMode = SDL_SCALEMODE_LINEAR;

        /**
         * @brief Map of all loaded audio streams by unique ID.
         *
         * Stored as weak_ptr to allow automatic cleanup when no shared_ptr
         * references remain.
         */
        static inline std::unordered_map<int, std::weak_ptr<AudioStream>> sounds{};

        /**
         * @brief Lookup map from file path to audio stream ID.
         *
         * Enables audio caching by file path. When loading audio,
         * this map is checked first to return existing streams.
         */
        static inline std::unordered_map<std::string, int> soundPathLookup{};

        /**
         * @brief Reverse lookup map from audio stream ID to file path.
         *
         * Used during cleanup to remove entries from soundPathLookup
         * when audio streams expire.
         */
        static inline std::unordered_map<int, std::string> soundIdToPath{};

      public:
        /**
         * @brief The SDL_mixer device for audio playback.
         *
         * Created during the first LoadSound() call and destroyed during Clean().
         *
         * @note Automatically initialized when needed.
         */
        static inline MIX_Mixer* mixer = nullptr;

        /**
         * @brief The number of audio tracks to create in the track pool.
         *
         * Determines how many sounds can play simultaneously. Default is 16.
         * Tracks are created lazily during the first LoadSound() call.
         *
         * @note Change this before loading any sounds to adjust the track count.
         */
        static inline uint8_t trackCount = 16;

        /**
         * @brief Map of audio tracks and their availability status.
         *
         * - **true**: Track is available for use
         * - **false**: Track is currently playing audio
         *
         * Used by AudioStream::Play() to find available tracks.
         */
        static inline std::unordered_map<MIX_Track*, bool> tracks{};

      public:
        /**
         * @brief Cleans up all resources and shuts down audio systems.
         *
         * Performs the following cleanup:
         * 1. Clears all sprite registrations
         * 2. Destroys all audio tracks
         * 3. Destroys the mixer device
         * 4. Shuts down SDL_mixer
         *
         * @note Called automatically by the Engine during shutdown.
         * @note Textures and audio streams are managed by weak_ptr and clean
         *       themselves up.
         */
        static void Clean();

      public:
        /**
         * @brief Deleted copy constructor to prevent copying.
         *
         * ResourceManager manages static resources and should not be copied.
         */
        ResourceManager(const ResourceManager& other) = delete;

        /**
         * @brief Registers a sprite for automatic rendering.
         *
         * Assigns a unique ID to the sprite and adds it to the Z-index sorted
         * render list. Called automatically by Sprite constructors.
         *
         * @param sprite Pointer to the sprite to register.
         * @return true if registration succeeded, false if the sprite is already
         *         registered (should not happen in normal use).
         *
         * @note Generates a random unique ID for the sprite.
         * @note Do not call manually; sprites register themselves.
         */
        static bool RegisterSprite(Sprite* sprite);

        /**
         * @brief Unregisters a sprite from automatic rendering.
         *
         * Removes the sprite from the render list. Called automatically by
         * Sprite destructors.
         *
         * @param spriteId The unique ID of the sprite to unregister.
         * @return true if unregistration succeeded, false if the sprite was not
         *         found.
         *
         * @note Do not call manually; sprites unregister themselves.
         */
        static bool UnregisterSprite(int spriteId);

        /**
         * @brief Updates a sprite's Z-index in the render order.
         *
         * Moves the sprite to a new position in the Z-index sorted render list.
         * Called automatically by Sprite::SetZIndex().
         *
         * @param sprite Pointer to the sprite.
         * @param newZIndex The new Z-index value.
         * @return true if the update succeeded, false if the sprite is not
         *         registered.
         *
         * @note Do not call manually; use Sprite::SetZIndex() instead.
         */
        static bool UpdateSpriteZIndex(Sprite* sprite, int newZIndex);

        /**
         * @brief Loads a texture from a file path.
         *
         * Checks the texture cache first. If the texture is already loaded,
         * returns the cached texture. Otherwise, loads from disk, caches it,
         * and returns the new texture.
         *
         * The texture is stored as weak_ptr and will be automatically cleaned
         * when no sprites reference it.
         *
         * @param filePath Path to the texture file.
         * @return Shared pointer to the texture, or nullptr if loading failed.
         *
         * @note Logs an error if loading fails.
         * @note Uses the current scaleMode setting.
         * @note Caches textures by file path to avoid duplicate loads.
         */
        static std::shared_ptr<SDL_Texture> LoadTexture(const std::string& filePath);

        /**
         * @brief Creates a blank texture programmatically.
         *
         * Useful for creating render targets or procedurally generated textures.
         * The texture is not cached by file path.
         *
         * @param format The SDL pixel format for the texture.
         * @param access The SDL texture access mode (e.g., SDL_TEXTUREACCESS_TARGET
         *               for render targets).
         * @param w Width of the texture in pixels.
         * @param h Height of the texture in pixels.
         * @return Shared pointer to the created texture.
         *
         * @note Uses the current scaleMode setting.
         * @note Not cached by file path (no file path exists).
         */
        static std::shared_ptr<SDL_Texture> CreateTexture(SDL_PixelFormat format, SDL_TextureAccess access, int w, int h);

        /**
         * @brief Cleans up expired (unused) resources.
         *
         * Iterates through textures and audio streams, removing those that are
         * no longer referenced (weak_ptr expired). This prevents memory leaks
         * from unused resources accumulating.
         *
         * @param maxPerCall Maximum number of resources to clean per type (textures
         *                   and audio). Prevents long cleanup times.
         *                   Default is 10.
         *
         * @note Called once per frame by the Engine.
         * @note Cleans both textures and audio streams.
         * @note Also removes entries from path lookup maps.
         */
        static void CleanExpired(size_t maxPerCall = 10);

        /**
         * @brief Renders all registered sprites sorted by Z-index.
         *
         * Iterates through sprites in Z-index order (lowest first) and queues
         * them for rendering via Renderer::BufferAdd(). Only sprites with
         * render=true are rendered.
         *
         * @param delta Delta time.
         *
         * @note Called once per frame by the Engine after scene rendering.
         * @note Uses the sprite's position and nullptr for camera (uses Camera::main).
         */
        static void RenderSprites(double delta);

        /**
         * @brief Sets the scale mode for newly loaded or created textures.
         *
         * This affects how textures are scaled when rendered at different sizes:
         * - **SDL_SCALEMODE_NEAREST**: Sharp, pixelated scaling (pixel art)
         * - **SDL_SCALEMODE_LINEAR**: Smooth scaling (default)
         * - **SDL_SCALEMODE_BEST**: Highest quality (may be slower)
         *
         * @param _scaleMode The SDL scale mode to use.
         *
         * @note Does not affect already-loaded textures.
         * @note Set this early (before loading textures) for best results.
         */
        static void SetScaleMode(SDL_ScaleMode _scaleMode);

        /**
         * @brief Loads an audio file for playback.
         *
         * Checks the audio cache first. If already loaded, returns the cached
         * audio stream. Otherwise, loads from disk, caches it, and returns
         * the new stream.
         *
         * Also initializes the mixer and creates the track pool if this is
         * the first audio load.
         *
         * @param filePath Path to the audio file.
         * @param properties SDL properties ID for playback customization.
         * @return Shared pointer to the AudioStream, or nullptr if loading failed.
         *
         * @note Initializes SDL_mixer on first call.
         * @note Creates the mixer and track pool lazily.
         * @note Caches audio by file path to avoid duplicate loads.
         * @note Logs an error if loading fails.
         */
        static std::shared_ptr<AudioStream> LoadSound(const std::string& filePath, SDL_PropertiesID properties);

        /**
         * @brief Gets the SDL_mixer device.
         *
         * @return Pointer to the mixer, or nullptr if not initialized.
         *
         * @note The mixer is created during the first LoadSound() call.
         */
        static MIX_Mixer* GetMixer();
    };
}