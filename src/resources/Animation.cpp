#include "igneous/resources/Animation.hpp"

#include "igneous/resources/ResourceManager.hpp"

namespace Engine
{
    Animation::Animation(const Animation& other)
        : name(other.name), frameIndex(other.frameIndex), fps(other.fps)
    {
        for (const auto& frame: other.frames)
        {
            frames.push_back(std::make_unique<AnimationFrame>(*frame));
        }
    }

    void Animation::SetName(const std::string& _name)
    {
        name = _name;
    }

    void Animation::SetFPS(float _fps)
    {
        fps = _fps;
    }

    void Animation::AddFPS(double delta)
    {
        currentFPS += delta;
        if (currentFPS > fps)
        {
            frameIndex++;
            if (frameIndex >= frames.size())
                frameIndex = 0;
            currentFPS -= fps;
        }
    }

    void Animation::AddFrame(const std::string& filePath)
    {
        frames.push_back(std::make_unique<AnimationFrame>());
        AnimationFrame* frame = frames.back().get();

        frame->texture = ResourceManager::LoadTexture(filePath);
        if (frame->texture)
        {
            float w, h;
            SDL_GetTextureSize(frame->texture.get(), &w, &h);
            frame->atlasW = w;
            frame->atlasH = h;
        }
    }

    void Animation::AddFrame(std::shared_ptr<SDL_Texture> texture)
    {
        frames.push_back(std::make_unique<AnimationFrame>());
        AnimationFrame* frame = frames.back().get();

        frame->texture = std::move(texture);
        if (frame->texture)
        {
            float w, h;
            SDL_GetTextureSize(frame->texture.get(), &w, &h);
            frame->atlasW = w;
            frame->atlasH = h;
        }
    }

    void Animation::AddFrame(const std::string& filePath, float _w, float _h, int _x, int _y)
    {
        frames.push_back(std::make_unique<AnimationFrame>());
        AnimationFrame* frame = frames.back().get();

        frame->texture = ResourceManager::LoadTexture(filePath);
        if (frame->texture)
        {
            frame->atlasW = _w;
            frame->atlasH = _h;
        }

        frame->atlasX = _x;
        frame->atlasY = _y;
    }

    void Animation::AddFrame(std::shared_ptr<SDL_Texture> text, float _w, float _h, int _x, int _y)
    {
        frames.push_back(std::make_unique<AnimationFrame>());
        AnimationFrame* frame = frames.back().get();

        frame->texture = std::move(text);
        if (frame->texture)
        {
            frame->atlasW = _w;
            frame->atlasH = _h;
        }

        frame->atlasX = _x;
        frame->atlasY = _y;
    }

    void Animation::AddFrame(const std::string& filePath, int row, int col, int rowPos, int colPos)
    {
        frames.push_back(std::make_unique<AnimationFrame>());
        AnimationFrame* frame = frames.back().get();

        frame->texture = ResourceManager::LoadTexture(filePath);
        if (frame->texture)
        {
            float w, h;
            SDL_GetTextureSize(frame->texture.get(), &w, &h);
            frame->atlasW = w / col;
            frame->atlasH = h / row;
        }

        frame->atlasX = colPos;
        frame->atlasY = rowPos;
    }

    void Animation::AddFrame(std::shared_ptr<SDL_Texture> text, int row, int col, int rowPos, int colPos)
    {
        frames.push_back(std::make_unique<AnimationFrame>());
        AnimationFrame* frame = frames.back().get();

        frame->texture = std::move(text);
        if (frame->texture)
        {
            float w, h;
            SDL_GetTextureSize(frame->texture.get(), &w, &h);
            frame->atlasW = w / col;
            frame->atlasH = h / row;
        }

        frame->atlasX = colPos;
        frame->atlasY = rowPos;
    }

    std::string Animation::GetName()
    {
        return name;
    }

    AnimationFrame* Animation::GetCurrentFrame()
    {
        if (frames.empty())
            return nullptr;
        if (frames.size() < frameIndex + 1)
            return nullptr;
        return frames[frameIndex].get();
    }

    int Animation::GetCurrentFrameIndex()
    {
        return frameIndex;
    }

    float Animation::GetCurrentFPS()
    {
        return currentFPS;
    }

    void Animation::SetCurrentFrameIndex(int index)
    {
        if (index < 0 || index >= frames.size())
            return;
        frameIndex = index;
    }

    void Animation::SetCurrentFPS(float _fps)
    {
        if (_fps > fps)
            _fps = fps;
        currentFPS = _fps;
    }

    std::vector<AnimationFrame*> Animation::GetFrames()
    {
        std::vector<AnimationFrame*> result;
        for (const auto& frame: frames)
        {
            result.push_back(frame.get());
        }
        return result;
    }
}