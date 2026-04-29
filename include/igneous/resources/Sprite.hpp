#pragma once

#include "Animation.hpp"

#include <string>
#include <memory>

#include "igneous/engine/Vec2.hpp"

#include <unordered_map>

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
     * ## Animation System
     *
     * Sprites hold a map of named animations. On construction, a "_default" animation
     * is always created from the supplied texture/path parameters. The current animation
     * can be switched via SetCurrentAnimation(). The active frame is retrieved through
     * GetCurrentFrame() and is used by the Renderer each frame.
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
     * @note Sprites require a position reference (Vec2<float>&) that they bind to.
     * @note Sprites are automatically registered/unregistered with the ResourceManager.
     * @note The position reference must outlive the sprite.
     *
     * @see ResourceManager
     * @see Renderer
     * @see Animation
     */
    class Sprite
    {
      private:
        /**
         * @brief A map of animations keyed by name.
         */
        std::unordered_map<std::string, std::unique_ptr<Animation>> animations{};

        /**
         * @brief The name of the animation currently playing.
         *
         * Defaults to "_default" on construction.
         * Use SetCurrentAnimation() to switch animations.
         */
        std::string currentAnimationName{};

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
         * @brief Whether to render the sprite centered on its position.
         *
         * - **true**: Position represents the sprite's center (default)
         * - **false**: Position represents the sprite's top-left corner
         */
        bool centered = true;

        /**
         * @brief Whether this sprite should be rendered.
         *
         * If false, the sprite is skipped during automatic rendering.
         */
        bool render = true;

        /**
         * @brief Horizontal scale multiplier.
         */
        float scaleX = 1.0f;

        /**
         * @brief Vertical scale multiplier.
         */
        float scaleY = 1.0f;

      public:
        /**
         * @brief Copy constructor. Deep-copies all animations and registers as a new sprite.
         *
         * Each Animation is deep-copied (frames duplicated, textures shared).
         * The new sprite gets its own unique ID and is registered with the ResourceManager.
         * The current animation name is preserved from the source sprite.
         *
         * @param sprite The sprite to copy. The position reference is shared.
         *
         * @note The position reference is copied (references the same Vec2).
         * @note The new sprite has independent transform and render state.
         */
        Sprite(Sprite& sprite);

        /**
         * @brief Constructs a sprite without a texture.
         *
         * @param pos Reference to the sprite's position. Must outlive the sprite.
         */
        Sprite(Vec2<float>& pos);
 /**
         * @brief Constructs a sprite from a texture file path.
         *
         * Creates a "_default" animation with a single frame sampling the entire texture.
         *
         * @param pos Reference to the sprite's position. Must outlive the sprite.
         * @param filePath Path to the texture file.
         */
        Sprite(Vec2<float>& pos, const std::string& filePath);

        /**
         * @brief Constructs a sprite from an existing texture shared_ptr.
         *
         * Creates a "_default" animation with a single frame sampling the entire texture.
         *
         * @param pos Reference to the sprite's position. Must outlive the sprite.
         * @param texture Shared pointer to the texture.
         */
        Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> texture);

        /**
         * @brief Constructs a sprite with explicit atlas settings from a texture file path.
         *
         * Creates a "_default" animation with a single frame at the given atlas region.
         *
         * @param pos Reference to the sprite's position. Must outlive the sprite.
         * @param filePath Path to the texture file.
         * @param _w Width of the sprite region in pixels.
         * @param _h Height of the sprite region in pixels.
         * @param _x X grid coordinate in the texture atlas.
         * @param _y Y grid coordinate in the texture atlas.
         */
        Sprite(Vec2<float>& pos, const std::string& filePath, float _w, float _h, int _x, int _y);

        /**
         * @brief Constructs a sprite with explicit atlas settings from an existing texture.
         *
         * Creates a "_default" animation with a single frame at the given atlas region.
         *
         * @param pos Reference to the sprite's position. Must outlive the sprite.
         * @param text Shared pointer to the texture.
         * @param _w Width of the sprite region in pixels.
         * @param _h Height of the sprite region in pixels.
         * @param _x X grid coordinate in the texture atlas.
         * @param _y Y grid coordinate in the texture atlas.
         */
        Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> text, float _w, float _h, int _x, int _y);

        /**
         * @brief Destructor that unregisters the sprite from the ResourceManager.
         */
        ~Sprite();

        /**
         * @brief Adds an animation with the given name.
         *
         * @param name Unique animation name.
         */
        Animation* AddAnimation(const std::string& name, bool setCurrent = false, bool keepIndex = false);

        /**
         * @brief Sets the currently active animation by name.
         *
         * If the name does not match any registered animation, the current
         * animation is unchanged.
         *
         * @param name The name of the animation to switch to.
         */
        void SetCurrentAnimation(const std::string& name, bool keepIndex = false);

        /**
         * @brief Gets the currently active animation.
         *
         * @return Pointer to the active Animation, or nullptr if none is set.
         */
        Animation* GetCurrentAnimation();

        /**
         * @brief Gets the current frame of the active animation.
         *
         * @return Pointer to the active AnimationFrame, or nullptr if no
         *         animation/frames exist.
         */
        AnimationFrame* GetCurrentFrame();

        /**
         * @brief Replaces the first frame of the "_default" animation's texture.
         *
         * Loads (or retrieves cached) texture from the file path.
         *
         * @param filePath Path to the new texture file.
         * @return true if the texture was loaded successfully, false otherwise.
         */
        bool SetTexture(const std::string& filePath);

        /**
         * @brief Gets the raw SDL_Texture pointer from the current animation's active frame.
         *
         * @return Pointer to the SDL_Texture, or nullptr if none is set.
         */
        SDL_Texture* GetTexture();

        /**
         * @brief Updates the atlas region on the current animation's active frame.
         *
         * @param w Width of the sprite region in pixels.
         * @param h Height of the sprite region in pixels.
         * @param x X grid coordinate in the texture atlas.
         * @param y Y grid coordinate in the texture atlas.
         */
        void SetAtlas(float w, float h, int x, int y);

        /**
         * @brief Sets the sprite's Z-index for rendering order.
         *
         * @param z The new z-index value.
         */
        void SetZIndex(int z);

        /**
         * @brief Gets the sprite's current Z-index.
         *
         * @return The z-index value.
         */
        int GetZIndex();

        /**
         * @brief Calculates the SDL source rectangle from the current animation frame.
         *
         * @return SDL_FRect defining the source region in the texture.
         */
        SDL_FRect GetSourceRect();

        /**
         * @brief Retrieves an animation by name.
         *
         * @param name The animation name.
         * @return Pointer to the Animation, or nullptr if not found.
         */
        Animation* GetAnimation(const std::string& name);

        /**
         * @brief Retrieves raw pointers to all animations on this sprite.
         *
         * @return Vector of Animation pointers.
         */
        std::vector<Animation*> GetAnimations();

        /**
         * @brief Checks if the mouse cursor is within the sprite's bounds.
         *
         * Uses the current frame's atlas dimensions and the sprite's scale.
         *
         * @param camera The camera to use. If nullptr, uses Camera::main.
         * @return true if the mouse is within the sprite's bounds.
         */
        bool IsMouseWithin(Camera* camera = nullptr);
    };
}