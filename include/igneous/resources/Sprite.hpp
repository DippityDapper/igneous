#pragma once

#include <string>
#include <memory>

#include "igneous/engine/Vec2.hpp"

namespace Engine
{
    class Camera;
}

struct SDL_Texture;
struct SDL_FRect;

namespace Engine
{
    /**
     * @class Sprite
     * @brief A renderable texture container with positioning, scaling, and atlas support.
     *
     * The Sprite class wraps an SDL_Texture and provides functionality for:
     * - Position-based rendering in world space
     * - Texture atlas sampling (sprite sheets)
     * - Z-index based layering
     * - Scaling and centering
     * - Mouse collision detection
     *
     * ## Texture Atlas System
     *
     * Sprites support texture atlases (sprite sheets) by allowing you to specify:
     * - **atlasW/atlasH**: The width/height of the sprite region to sample
     * - **atlasX/atlasY**: The grid coordinates within the texture to sample from
     *
     * For example, a 256x256 texture with 32x32 sprites would have 8x8 grid positions.
     * To sample the sprite at grid position (2, 3), set atlasX=2, atlasY=3, with
     * atlasW=32, atlasH=32.
     *
     * ## Automatic Rendering
     *
     * Sprites are automatically registered with the ResourceManager and rendered
     * each frame if their render flag is true. Z-index determines draw order
     * (higher values draw on top).
     *
     * ## Texture Lifetime
     *
     * Sprites use shared_ptr for texture ownership, allowing multiple sprites to
     * reference the same texture. The ResourceManager caches textures by file path
     * and handles cleanup when no sprites reference them.
     *
     * @note Sprites require a position reference (Vec2<float>&) that they bind to.
     * @note Sprites are automatically registered/unregistered with the ResourceManager.
     * @note The position reference must outlive the sprite.
     *
     * @see ResourceManager
     * @see Renderer
     */
    class Sprite
    {
    private:
        /**
         * @brief Shared pointer to the SDL texture.
         *
         * Multiple sprites can share the same texture. The texture is kept alive
         * as long as at least one sprite or other shared_ptr references it.
         * The ResourceManager caches textures and handles cleanup.
         *
         * @note Lifetime is managed by the ResourceManager.
         */
        std::shared_ptr<SDL_Texture> texture = nullptr;

        /**
         * @brief Z-index for rendering order.
         *
         * Higher z-index values are rendered on top of lower values.
         * Modified via SetZIndex(), which updates the ResourceManager's
         * sprite rendering order.
         */
        int zIndex = 0;

    public:
        /**
         * @brief Unique identifier assigned by the ResourceManager.
         *
         * Automatically assigned during construction when the sprite is
         * registered. Used internally for sprite management.
         *
         * @note Do not modify manually.
         */
        int id = 0;

        /**
         * @brief Reference to the sprite's world position.
         *
         * The sprite binds to this position reference and uses it for rendering.
         * The reference must remain valid for the lifetime of the sprite.
         *
         * @note This is a reference, not a value. Changes to the referenced
         *       Vec2 automatically affect the sprite's position.
         */
        Vec2<float>& position;

        /**
         * @brief The full width of the texture in pixels.
         *
         * Automatically set during construction by querying the texture dimensions.
         * Represents the entire texture, not the sampled region.
         */
        float w = 0;

        /**
         * @brief The full height of the texture in pixels.
         *
         * Automatically set during construction by querying the texture dimensions.
         * Represents the entire texture, not the sampled region.
         */
        float h = 0;

        /**
         * @brief The width to sample from the texture atlas in pixels.
         *
         * Defines the width of the region to extract from the texture.
         * For non-atlas sprites, this equals the full texture width.
         * For atlas sprites, this is the width of a single sprite cell.
         */
        float atlasW = 0;

        /**
         * @brief The height to sample from the texture atlas in pixels.
         *
         * Defines the height of the region to extract from the texture.
         * For non-atlas sprites, this equals the full texture height.
         * For atlas sprites, this is the height of a single sprite cell.
         */
        float atlasH = 0;

        /**
         * @brief The X grid coordinate to sample from the texture atlas.
         *
         * Used with atlasW to calculate the pixel offset: pixelX = atlasX * atlasW.
         * For non-atlas sprites, this is typically 0.
         *
         * Example: For a sprite sheet with 32px cells, atlasX=2 samples at pixel 64.
         */
        int atlasX = 0;

        /**
         * @brief The Y grid coordinate to sample from the texture atlas.
         *
         * Used with atlasH to calculate the pixel offset: pixelY = atlasY * atlasH.
         * For non-atlas sprites, this is typically 0.
         *
         * Example: For a sprite sheet with 32px cells, atlasY=3 samples at pixel 96.
         */
        int atlasY = 0;

        /**
         * @brief Whether to render the sprite centered on its position.
         *
         * - **true**: Position represents the sprite's center (default)
         * - **false**: Position represents the sprite's top-left corner
         *
         * This also affects how scaling is applied. Centered sprites scale
         * symmetrically around the position, while non-centered sprites scale
         * from the top-left corner.
         */
        bool centered = true;

        /**
         * @brief Whether this sprite should be rendered.
         *
         * If false, the sprite is skipped during automatic rendering.
         * Useful for temporarily hiding sprites without destroying them.
         */
        bool render = true;

        /**
         * @brief Horizontal scale multiplier.
         *
         * Applied to the sprite's width during rendering.
         * Values < 1.0 shrink, > 1.0 enlarge, negative values flip horizontally.
         */
        float scaleX = 1.0f;

        /**
         * @brief Vertical scale multiplier.
         *
         * Applied to the sprite's height during rendering.
         * Values < 1.0 shrink, > 1.0 enlarge, negative values flip vertically.
         */
        float scaleY = 1.0f;

    public:
        /**
         * @brief Copy constructor that creates a sprite sharing the same texture.
         *
         * Creates a new sprite that references the same texture as the source sprite.
         * All atlas settings and dimensions are copied, but the new sprite gets
         * its own unique ID and is registered separately with the ResourceManager.
         *
         * @param sprite The sprite to copy.
         *
         * @note The new sprite shares the texture but has independent transform/render state.
         * @note The position reference is copied (references the same Vec2).
         */
        Sprite(const Sprite& sprite);

        /**
         * @brief Constructs a sprite from a texture file path.
         *
         * Loads (or retrieves cached) texture from the file path and creates a sprite
         * that renders the entire texture. The atlas dimensions are set to match
         * the full texture size.
         *
         * @param pos Reference to the sprite's position. Must outlive the sprite.
         * @param filePath Path to the texture file.
         *
         * @note If the texture fails to load, the sprite will have no texture.
         * @note Atlas settings default to rendering the entire texture.
         */
        Sprite(Vec2<float>& pos, const std::string& filePath);

        /**
         * @brief Constructs a sprite from an existing texture shared_ptr.
         *
         * Creates a sprite using an already-loaded texture. The atlas dimensions
         * are set to match the full texture size.
         *
         * @param pos Reference to the sprite's position. Must outlive the sprite.
         * @param text Shared pointer to the texture.
         *
         * @note Useful when creating multiple sprites from a programmatically
         *       generated texture.
         */
        Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> text);

        /**
         * @brief Constructs a sprite with atlas settings from a texture file path.
         *
         * Loads (or retrieves cached) texture and creates a sprite that renders
         * a specific region of the texture defined by atlas parameters.
         *
         * @param pos Reference to the sprite's position. Must outlive the sprite.
         * @param filePath Path to the texture file.
         * @param _w Width of the sprite region to sample (atlasW).
         * @param _h Height of the sprite region to sample (atlasH).
         * @param _x X grid coordinate in the texture atlas (atlasX).
         * @param _y Y grid coordinate in the texture atlas (atlasY).
         *
         * @note If the texture fails to load, the sprite will have no texture.
         *
         * Example:
         * @code
         * // Sample a 32x32 sprite at grid position (2, 3) from "sheet.png"
         * Sprite sprite(position, "sheet.png", 32, 32, 2, 3);
         * @endcode
         */
        Sprite(Vec2<float>& pos, const std::string& filePath, float _w, float _h, int _x, int _y);

        /**
         * @brief Constructs a sprite with atlas settings from an existing texture.
         *
         * Creates a sprite using an already-loaded texture and renders a specific
         * region defined by atlas parameters.
         *
         * @param pos Reference to the sprite's position. Must outlive the sprite.
         * @param text Shared pointer to the texture.
         * @param _w Width of the sprite region to sample (atlasW).
         * @param _h Height of the sprite region to sample (atlasH).
         * @param _x X grid coordinate in the texture atlas (atlasX).
         * @param _y Y grid coordinate in the texture atlas (atlasY).
         */
        Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> text, float _w, float _h, int _x, int _y);

        /**
         * @brief Destructor that unregisters the sprite from the ResourceManager.
         *
         * Automatically removes the sprite from the rendering system.
         * The texture is not destroyed if other sprites reference it.
         */
        ~Sprite();

        /**
         * @brief Replaces the sprite's texture with a new one from a file path.
         *
         * Loads (or retrieves cached) texture from the file path and updates
         * the sprite to use it. Does not modify atlas settings.
         *
         * @param filePath Path to the new texture file.
         * @return true if the texture was loaded successfully, false otherwise.
         *
         * @note The old texture is released if no other sprites reference it.
         */
        bool SetTexture(const std::string& filePath);

        /**
         * @brief Gets the raw SDL_Texture pointer.
         *
         * @return Pointer to the SDL_Texture, or nullptr if no texture is set.
         *
         * @note Handle with care - the shared_ptr owns the texture.
         */
        SDL_Texture* GetTexture();

        /**
         * @brief Updates the texture atlas sampling region.
         *
         * Sets the dimensions and grid coordinates for sampling from a texture atlas.
         *
         * @param w Width of the sprite region to sample (atlasW).
         * @param h Height of the sprite region to sample (atlasH).
         * @param x X grid coordinate in the texture atlas (atlasX).
         * @param y Y grid coordinate in the texture atlas (atlasY).
         *
         * Example:
         * @code
         * // Change to sample sprite at grid position (4, 1)
         * sprite.SetAtlas(32, 32, 4, 1);
         * @endcode
         */
        void SetAtlas(float w, float h, int x, int y);

        /**
         * @brief Sets the sprite's Z-index for rendering order.
         *
         * Updates the sprite's position in the ResourceManager's rendering
         * order. Higher z-index values are rendered on top of lower values.
         *
         * @param z The new z-index value.
         *
         * @note No-op if the z-index is already set to this value.
         * @note Updates the ResourceManager's sprite ordering.
         */
        void SetZIndex(int z);

        /**
         * @brief Gets the sprite's current Z-index.
         *
         * @return The z-index value used for rendering order.
         */
        int GetZIndex();

        /**
         * @brief Calculates the SDL source rectangle for texture sampling.
         *
         * Converts the atlas grid coordinates and dimensions into pixel coordinates
         * for SDL's texture rendering. Used internally by the Renderer.
         *
         * @return SDL_FRect defining the source region in the texture.
         *
         * @note Pixel coordinates are calculated as: x = atlasX * atlasW, y = atlasY * atlasH.
         */
        SDL_FRect GetSourceRect() const;

        /**
         * @brief Checks if the mouse cursor is within the sprite's bounds.
         *
         * Tests whether the mouse position (in world space) intersects with
         * the sprite's bounding box. The bounds are calculated using the sprite's
         * position, atlas dimensions, and scale.
         *
         * The sprite is always treated as centered for this calculation, using
         * its center position with half-width/height offsets.
         *
         * @param camera The camera to use for mouse world position calculation.
         *               If nullptr, uses Camera::main.
         *
         * @return true if the mouse is within the sprite's bounds, false otherwise.
         *
         * @note Uses Camera::GetMouseGlobalPosition() to convert screen to world space.
         * @note Takes scaleX and scaleY into account for bounds calculation.
         */
        bool IsMouseWithin(Camera* camera = nullptr);
    };
}