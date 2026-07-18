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

    class Sprite
    {
      private:

        std::unordered_map<std::string, std::unique_ptr<Animation>> animations{};

        std::string currentAnimationName{};

        int zIndex = 0;

      public:

        int id = 0;

        Vec2<float>& position;

        bool centered = true;

        bool render = true;

        float scaleX = 1.0f;

        float scaleY = 1.0f;

      public:

        Sprite(Sprite& sprite);

        Sprite(Vec2<float>& pos);

        Sprite(Vec2<float>& pos, const std::string& filePath);

        Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> texture);

        Sprite(Vec2<float>& pos, const std::string& filePath, float _w, float _h, int _x, int _y);

        Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> text, float _w, float _h, int _x, int _y);

        ~Sprite();

        Animation* AddAnimation(const std::string& name, bool setCurrent = false, bool keepIndex = false);

        void SetCurrentAnimation(const std::string& name, bool keepIndex = false);

        Animation* GetCurrentAnimation();

        AnimationFrame* GetCurrentFrame();

        bool SetTexture(const std::string& filePath);

        SDL_Texture* GetTexture();

        void SetAtlas(float w, float h, int x, int y);

        void SetZIndex(int z);

        int GetZIndex();

        SDL_FRect GetSourceRect();

        Animation* GetAnimation(const std::string& name);

        std::vector<Animation*> GetAnimations();

        bool IsMouseWithin(Camera* camera = nullptr);
    };
}
