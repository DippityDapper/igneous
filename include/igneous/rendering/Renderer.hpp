#pragma once

struct SDL_Renderer;
struct SDL_Texture;

namespace Engine
{
    template<typename T>
    struct Vec2;
    class Sprite;
    class Camera;

    /**
     * @class Renderer
     * @brief Manages SDL3 rendering and ImGui integration.
     *
     * The Renderer class provides a centralized interface for all rendering operations
     * in the engine. It manages the SDL3 renderer, ImGui context, and provides methods
     * for queuing sprites and textures to be drawn to the screen.
     *
     * The renderer supports both world-space rendering (with camera transformations)
     * and screen-space rendering (direct pixel coordinates). All rendering operations
     * are queued via BufferAdd methods and presented together during Render().
     *
     * @note This class uses a static SDL_Renderer instance and should not be copied.
     * @note The renderer must be initialized after the Window is created.
     * @note ImGui rendering is integrated into the same render pipeline.
     */
    class Renderer
    {
    private:
        /**
         * @brief The static SDL3 renderer instance.
         *
         * Shared across all rendering operations. Created during Init() and
         * destroyed during Clean().
         */
        static SDL_Renderer* renderer;

    public:
        /**
         * @brief Deleted copy constructor to prevent copying.
         *
         * The Renderer manages static resources and should not be copied.
         */
        Renderer(const Renderer& other) = delete;

        /**
         * @brief Initializes the SDL3 renderer and ImGui context.
         *
         * Creates the SDL renderer from the initialized Window, then sets up:
         * - ImGui context
         * - ImGui dark style theme
         * - ImGui SDL3 backend
         * - ImGui SDL_Renderer3 backend
         *
         * @note The Window must be initialized before calling this method.
         * @note Logs an error if the window is not found or renderer creation fails.
         * @note Called automatically by the Engine during initialization.
         */
        static void Init();

        /**
         * @brief Renders ImGui draw data and presents the final frame.
         *
         * Performs the following operations:
         * 1. Renders all ImGui draw data to the renderer
         * 2. Presents the complete frame to the screen (SDL_RenderPresent)
         *
         * @note Called once per frame by the Engine after all BufferAdd calls.
         * @note All sprite/texture rendering via BufferAdd happens before this call.
         */
        static void Render();

        /**
         * @brief Cleans up the renderer and ImGui resources.
         *
         * Shutdown order:
         * 1. ImGui SDL_Renderer3 backend
         * 2. ImGui SDL3 backend
         * 3. ImGui context
         * 4. SDL renderer
         *
         * @note Called automatically by the Engine during shutdown.
         */
        static void Clean();

        /**
         * @brief Clears the render buffer and begins a new ImGui frame.
         *
         * Prepares the renderer for a new frame by:
         * 1. Setting the draw color to black (0, 0, 0, 255)
         * 2. Clearing the entire render target
         * 3. Starting a new ImGui SDL_Renderer3 frame
         * 4. Starting a new ImGui SDL3 frame
         * 5. Beginning a new ImGui frame
         *
         * @note Called once per frame by the Engine at the start of the render cycle.
         * @note All BufferAdd calls should occur after this and before Render().
         */
        static void BufferClear();

        /**
         * @brief Adds a sprite to the render queue in world space.
         *
         * Renders a sprite with full camera transformation, including:
         * - World-to-screen position transformation
         * - Camera zoom scaling
         * - Sprite scale (scaleX, scaleY)
         * - Sprite atlas source rectangle
         * - Optional sprite centering
         *
         * The sprite's position is transformed from world space to screen space
         * using the provided camera. If the sprite's texture is null or the
         * renderer is not initialized, the call is silently ignored.
         *
         * @param position World space position to render the sprite.
         * @param sprite The sprite to render. Must have a valid texture.
         * @param camera The camera used for world-to-screen transformation.
         *               If nullptr, uses Camera::main. If Camera::main is also
         *               nullptr, the call is ignored.
         *
         * @note Respects the sprite's centered flag for positioning.
         * @note Uses the sprite's atlas coordinates (atlasW, atlasH) and scale.
         * @note Silently fails if viewport dimensions are <= 0.
         */
        static void BufferAdd(Vec2<float> position, Sprite* sprite, Camera* camera);

        /**
         * @brief Adds a texture to the render queue in world space.
         *
         * Renders a raw SDL_Texture with camera transformation, including:
         * - World-to-screen position transformation
         * - Camera zoom scaling
         * - Optional centering
         *
         * Unlike the sprite overload, this uses the full texture dimensions
         * without atlas or sprite-specific scaling.
         *
         * @param position World space position to render the texture.
         * @param texture The SDL_Texture to render. If nullptr, the call is ignored.
         * @param camera The camera used for world-to-screen transformation.
         *               If nullptr, uses Camera::main. If Camera::main is also
         *               nullptr, the call is ignored.
         * @param centered If true, centers the texture on the position.
         *                 If false, renders with position as top-left corner.
         *                 Default is true.
         *
         * @note Uses the texture's native dimensions (texture->w, texture->h).
         * @note Silently fails if viewport dimensions are <= 0.
         */
        static void BufferAdd(Vec2<float> position, SDL_Texture* texture, Camera* camera, bool centered = true);

        /**
         * @brief Adds a sprite to the render queue in screen space.
         *
         * Renders a sprite directly in screen-space coordinates without any
         * camera transformation. The position is treated as pixel coordinates
         * relative to the viewport.
         *
         * This overload applies:
         * - Sprite scale (scaleX, scaleY)
         * - Sprite atlas source rectangle
         * - No camera transformations
         * - No centering (position is top-left corner)
         *
         * @param position Screen space position (pixel coordinates) to render the sprite.
         * @param sprite The sprite to render. Must have a valid texture.
         *
         * @note Useful for UI elements, HUD, or other screen-fixed graphics.
         * @note Silently fails if sprite or sprite texture is null.
         * @note Does not apply the sprite's centered flag.
         */
        static void BufferAdd(Vec2<float> position, Sprite* sprite);

        /**
         * @brief Gets the static SDL_Renderer instance.
         *
         * Provides access to the underlying SDL renderer for advanced rendering
         * operations not covered by the BufferAdd methods.
         *
         * @return Pointer to the SDL_Renderer, or nullptr if not initialized.
         *
         * @note Handle with care - direct manipulation can interfere with the
         *       engine's rendering pipeline.
         */
        static SDL_Renderer* GetRenderer();
    };
}