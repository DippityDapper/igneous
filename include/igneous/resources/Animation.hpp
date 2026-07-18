// Doc: docs/classes/Animation.md
#pragma once
#include "igneous/resources/AnimationFrame.hpp"

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

        float fps = 5.0f;

        float fpsDelta = 0.2f;

        float elapsedTime = 0.0f;

        std::vector<std::unique_ptr<AnimationFrame>> frames{};

      public:

        Animation() = default;

        Animation(const Animation& other);

        void SetName(const std::string& _name);

        void SetFPS(float _fps);

        void IncrementElapsedTime(double delta);

        void AddFrame(const std::string& filePath);

        void AddFrame(std::shared_ptr<SDL_Texture> texture);

        void AddFrame(const std::string& filePath, float _w, float _h, int _x, int _y);

        void AddFrame(std::shared_ptr<SDL_Texture> text, float _w, float _h, int _x, int _y);

        void AddFrame(const std::string& filePath, int row, int col, int rowPos, int colPos);

        void AddFrame(std::shared_ptr<SDL_Texture> text, int row, int col, int rowPos, int colPos);

        std::string GetName();

        AnimationFrame* GetCurrentFrame();

        int GetCurrentFrameIndex();

        float GetElapsedTime();

        void SetCurrentFrameIndex(int index);

        void SetElapsedTime(float newTime);

        std::vector<AnimationFrame*> GetFrames();
    };
}
