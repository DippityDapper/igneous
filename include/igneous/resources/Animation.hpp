#pragma once
#include "AnimationFrame.hpp"

#include <memory>
#include <vector>
#include <string>

struct SDL_Texture;

namespace Engine
{
    class Animation
    {
      private:
        std::string name{};
        int frameIndex = 0;
        float fps = 0.3f;
        float currentFPS = 0.0f;
        std::vector<std::unique_ptr<AnimationFrame>> frames{};

      public:
        Animation() = default;

        /**
         * @brief Copy constructor. Deep-copies all frames, sharing the underlying textures.
         *
         * Each AnimationFrame is duplicated, but the SDL_Texture shared_ptr is shared,
         * so no texture data is duplicated.
         *
         * @param other The animation to copy.
         */
        Animation(const Animation& other);

        /**
         * @brief Sets the name of the animation.
         *
         * @param name String name.
         */
        void SetName(const std::string& _name);

        /**
         * @brief Sets the playback speed of the animation in frames per second.
         *
         * @param _fps Frames per second.
         */
        void SetFPS(float _fps);

        /**
         * @brief Adds delta time to the current FPS.
         *
         * @param delta Delta time.
         */
        void AddFPS(double delta);

        /**
         * @brief Adds a frame from a texture file, sampling the entire texture.
         *
         * @param filePath Path to the texture file.
         */
        void AddFrame(const std::string& filePath);

        /**
         * @brief Adds a frame from an existing texture, sampling the entire texture.
         *
         * @param texture Shared pointer to the SDL texture.
         */
        void AddFrame(std::shared_ptr<SDL_Texture> texture);

        /**
         * @brief Adds a frame from a texture file using explicit atlas grid coordinates.
         *
         * @param filePath Path to the texture file.
         * @param _w Width of the sprite region in pixels.
         * @param _h Height of the sprite region in pixels.
         * @param _x X grid coordinate in the atlas.
         * @param _y Y grid coordinate in the atlas.
         */
        void AddFrame(const std::string& filePath, float _w, float _h, int _x, int _y);

        /**
         * @brief Adds a frame from an existing texture using explicit atlas grid coordinates.
         *
         * @param text Shared pointer to the SDL texture.
         * @param _w Width of the sprite region in pixels.
         * @param _h Height of the sprite region in pixels.
         * @param _x X grid coordinate in the atlas.
         * @param _y Y grid coordinate in the atlas.
         */
        void AddFrame(std::shared_ptr<SDL_Texture> text, float _w, float _h, int _x, int _y);

        /**
         * @brief Adds a frame from a texture file using row/column grid division.
         *
         * The texture is divided evenly into a grid of @p col columns and @p row rows.
         * The cell at (@p colPos, @p rowPos) is sampled.
         *
         * @param filePath Path to the texture file.
         * @param row Total number of rows in the texture grid.
         * @param col Total number of columns in the texture grid.
         * @param rowPos Row index of the cell to sample (0-based).
         * @param colPos Column index of the cell to sample (0-based).
         */
        void AddFrame(const std::string& filePath, int row, int col, int rowPos, int colPos);

        /**
         * @brief Adds a frame from an existing texture using row/column grid division.
         *
         * The texture is divided evenly into a grid of @p col columns and @p row rows.
         * The cell at (@p colPos, @p rowPos) is sampled.
         *
         * @param text Shared pointer to the SDL texture.
         * @param row Total number of rows in the texture grid.
         * @param col Total number of columns in the texture grid.
         * @param rowPos Row index of the cell to sample (0-based).
         * @param colPos Column index of the cell to sample (0-based).
         */
        void AddFrame(std::shared_ptr<SDL_Texture> text, int row, int col, int rowPos, int colPos);

        /**
         * @brief Returns the name of the animation.
         *
         * @return A string of the animation name.
         */
        std::string GetName();

        /**
         * @brief Returns the raw pointer to the current frame.
         *
         * @return A pointer to the current frame.
         */
        AnimationFrame* GetCurrentFrame();

        /**
         * @brief Returns the index to the current frame.
         *
         * @return An integer of the index of the current frame.
         */
        int GetCurrentFrameIndex();

        /**
         * @brief Returns the current fps.
         *
         * @return A float of the current fps.
         */
        float GetCurrentFPS();

        /**
         * @brief Sets the index for the current frame.
         *
         * @param index The new index.
         */
        void SetCurrentFrameIndex(int index);

        /**
         * @brief Sets the current fps.
         *
         * @param _fps The new fps.
         */
        void SetCurrentFPS(float _fps);

        /**
         * @brief Returns raw pointers to all frames in this animation.
         *
         * @return Vector of AnimationFrame pointers. Pointers are valid for
         *         the lifetime of the Animation.
         */
        std::vector<AnimationFrame*> GetFrames();
    };
}