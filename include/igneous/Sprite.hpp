#pragma once

#include <string>
#include <memory>

#include "igneous/Vec2.hpp"

namespace Engine
{
    class Camera;
}

struct SDL_Texture;
struct SDL_FRect;

namespace Engine
{
    /// A container that holds an SDL texture.
    class Sprite
    {
    private:
        /// A shared pointer to the SDL texture.
        /// @note The texture's lifetime is handled by the ResourceLoader.
        std::shared_ptr<SDL_Texture> texture = nullptr;

        int zIndex = 0;

    public:
        int id = 0;

        Vec2<float>& position;

        /// The width of the entire texture. In pixels.
        float w = 0;

        /// The height of the entire texture. In pixels.
        float h = 0;

        /// The width to sample from the texture. In pixels.
        float atlasW = 0;

        /// The height to sample from the texture. In pixels.
        float atlasH = 0;

        /// The x position to sample from the texture. In pixels.
        int atlasX = 0;

        /// The y position to sample from the texture. In pixels.
        int atlasY = 0;

        /// Determines if the texture will be rendered from its center. This effects scale direction.
        bool centered = true;

        bool render = true;

        /// The scale of the texture.
        float scaleX = 1.0f;
        float scaleY = 1.0f;

    public:
        /// Creates a copy of a sprite.
        /// The texture of the new sprite will point to the same texture of the old sprite.
        Sprite(const Sprite& sprite);

        /// Creates a sprite from a file path.
        /// The width and height of the sprite is set to the texture's size.
        /// The atlas width and height are set to the texture size, meaning the whole texture will be rendered.
        /// The atlas x and y position are set to 0.
        /// @param filePath The file path to the texture.
        Sprite(Vec2<float>& pos, const std::string& filePath);

        Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> text);

        /// Creates a sprite from a file path.
        /// @param filePath The file path to the texture.
        /// @param _w The atlas width to sample from the main texture.
        /// @param _h The atlas height to sample from the main texture.
        /// @param _x The atlas x position to sample from the main texture.
        /// @param _y The atlas y position to sample from the main texture.
        Sprite(Vec2<float>& pos, const std::string& filePath, float _w, float _h, int _x, int _y);

        Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> text, float _w, float _h, int _x, int _y);

        ~Sprite();

        bool SetTexture(const std::string& filePath);

        /// Gets a pointer to the SDL texture.
        /// @returns A pointer to the SDL texture.
        SDL_Texture* GetTexture();

        void SetAtlas(float w, float h, int x, int y);

        void SetZIndex(int z);

        int GetZIndex();

        /// Gets the source rect of the texture.
        /// @returns The source rect of the texture.
        SDL_FRect GetSourceRect() const;

        bool IsMouseWithin(Camera* camera = nullptr);
    };
}
