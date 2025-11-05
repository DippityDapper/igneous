#pragma once

struct SDL_Renderer;
struct SDL_Texture;

namespace Engine
{
    template<typename T>
    struct Vec2;
    class Sprite;
    class Camera;

    /// A utility class that contains the renderer.
    class Renderer
    {
    private:
        /// The SDL renderer object.
        static SDL_Renderer* renderer;

    public:
        Renderer(const Renderer& other) = delete;

        /// Initializes the renderer and ImGui. If the window is not initialized, this will fail.
        /// @note This is called within the Engine.
        void Init();

        /// Calls to present the current render.
        /// @note This is called within the Engine.
        void Render();

        /// Cleans the renderer and ImGui.
        /// @note This is called within the Engine.
        void Clean();

        /// Clears the buffer of the renderer and ImGui to be drawn into.
        static void BufferClear();

        /// Adds a sprite's texture to the next draw pass.
        /// @param position The world space position to draw the sprite.
        /// @param sprite The sprite to draw the texture of.
        /// @param camera The camera to use when calculating the screen position for the rendered texture.
        /// If no camera is provided, the main camera will be used.
        static void BufferAdd(Vec2<float> position, Sprite* sprite, Camera* camera);

        /// Adds a texture to the next draw pass.
        /// @param position The world space position to draw the sprite.
        /// @param texture The texture to draw.
        /// @param camera The camera to use when calculating the screen position for the rendered texture.
        /// If no camera is provided, the main camera will be used.
        static void BufferAdd(Vec2<float> position, SDL_Texture* texture, Camera* camera, bool centered = true);

        /// Adds a sprite's texture to the next draw pass.
        /// @param position The screen space position to draw the sprite.
        /// @param sprite The sprite to draw the texture of.
        /// @note The camera is not used in the screen space calculation.
        static void BufferAdd(Vec2<float> position, Sprite* sprite);

        /// Gets a pointer to the SDL renderer.
        /// @returns A pointer to the SDL renderer.
        static SDL_Renderer* GetRenderer();
    };
}
