#include <utility>

#include "igneous/resources/Sprite.hpp"

#include "igneous/engine/Camera.hpp"
#include "SDL3/SDL.h"
#include "igneous/input/Input.hpp"
#include "igneous/rendering/Window.hpp"

#include "igneous/resources/ResourceManager.hpp"

#include <ranges>

namespace Engine
{
    Sprite::Sprite(Sprite& sprite) : position(sprite.position)
    {
        for (const auto& [animName, anim]: sprite.animations)
        {
            animations.emplace(animName, std::make_unique<Animation>(*anim));
        }

        currentAnimationName = sprite.currentAnimationName;
        scaleX = sprite.scaleX;
        scaleY = sprite.scaleY;
        centered = sprite.centered;
        render = sprite.render;
        zIndex = sprite.zIndex;

        ResourceManager::RegisterSprite(this);
    }

    Sprite::Sprite(Vec2<float>& pos) : position(pos)
    {
        ResourceManager::RegisterSprite(this);
    }

    Sprite::Sprite(Vec2<float>& pos, const std::string& filePath) : position(pos)
    {
        animations.emplace("_default", std::make_unique<Animation>());
        animations["_default"]->AddFrame(filePath);
        currentAnimationName = "_default";

        ResourceManager::RegisterSprite(this);
    }

    Sprite::Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> texture) : position(pos)
    {
        animations.emplace("_default", std::make_unique<Animation>());
        animations["_default"]->AddFrame(std::move(texture));
        currentAnimationName = "_default";

        ResourceManager::RegisterSprite(this);
    }

    Sprite::Sprite(Vec2<float>& pos, const std::string& filePath, float _w, float _h, int _x, int _y) : position(pos)
    {
        animations.emplace("_default", std::make_unique<Animation>());
        animations["_default"]->AddFrame(filePath, _w, _h, _x, _y);
        currentAnimationName = "_default";

        ResourceManager::RegisterSprite(this);
    }

    Sprite::Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> text, float _w, float _h, int _x, int _y) : position(pos)
    {
        animations.emplace("_default", std::make_unique<Animation>());
        animations["_default"]->AddFrame(std::move(text), _w, _h, _x, _y);
        currentAnimationName = "_default";

        ResourceManager::RegisterSprite(this);
    }

    Sprite::~Sprite()
    {
        ResourceManager::UnregisterSprite(id);
    }

    Animation* Sprite::AddAnimation(const std::string& name, bool setCurrent, bool keepIndex)
    {
        if (animations.contains(name))
            return nullptr;
        animations.emplace(name, std::make_unique<Animation>());

        if (setCurrent)
            SetCurrentAnimation(name, keepIndex);

        animations[name]->SetName(name);
        return animations[name].get();
    }

    void Sprite::SetCurrentAnimation(const std::string& name, bool keepIndex)
    {
        if (!animations.contains(name))
            return;

        int index = 0;
        if (keepIndex && GetCurrentAnimation())
            index = GetCurrentAnimation()->GetCurrentFrameIndex();

        float fps = 0;
        if (keepIndex && GetCurrentAnimation())
            fps = GetCurrentAnimation()->GetCurrentFPS();

        currentAnimationName = name;
        GetCurrentAnimation()->SetCurrentFrameIndex(index);
        GetCurrentAnimation()->SetCurrentFPS(fps);
    }

    Animation* Sprite::GetCurrentAnimation()
    {
        if (!animations.contains(currentAnimationName))
            return nullptr;

        return animations[currentAnimationName].get();
    }

    AnimationFrame* Sprite::GetCurrentFrame()
    {
        Animation* anim = GetCurrentAnimation();
        if (!anim)
            return nullptr;

        AnimationFrame* frame = anim->GetCurrentFrame();
        if (!frame)
            return nullptr;

        return frame;
    }

    bool Sprite::SetTexture(const std::string& filePath)
    {
        AnimationFrame* frame = GetCurrentFrame();
        if (!frame)
            return false;

        std::shared_ptr<SDL_Texture> newTexture = ResourceManager::LoadTexture(filePath);
        if (!newTexture)
            return false;

        frame->texture = newTexture;
        return true;
    }

    SDL_Texture* Sprite::GetTexture()
    {
        AnimationFrame* frame = GetCurrentFrame();
        if (!frame)
            return nullptr;

        return frame->texture.get();
    }

    void Sprite::SetAtlas(float w, float h, int x, int y)
    {
        AnimationFrame* frame = GetCurrentFrame();
        if (!frame)
            return;

        frame->atlasW = w;
        frame->atlasH = h;
        frame->atlasX = x;
        frame->atlasY = y;
    }

    void Sprite::SetZIndex(int z)
    {
        if (zIndex == z)
            return;

        zIndex = z;
        ResourceManager::UpdateSpriteZIndex(this, z);
    }

    int Sprite::GetZIndex()
    {
        return zIndex;
    }

    SDL_FRect Sprite::GetSourceRect()
    {
        Animation* animation = GetCurrentAnimation();
        if (!animation)
            return SDL_FRect{};

        AnimationFrame* frame = animation->GetCurrentFrame();
        if (!frame)
            return SDL_FRect{};

        SDL_FRect src;
        src.w = frame->atlasW;
        src.h = frame->atlasH;
        src.x = static_cast<float>(frame->atlasX) * frame->atlasW;
        src.y = static_cast<float>(frame->atlasY) * frame->atlasH;
        return src;
    }

    Animation* Sprite::GetAnimation(const std::string& name)
    {
        if (!animations.contains(name))
            return nullptr;
        return animations[name].get();
    }

    std::vector<Animation*> Sprite::GetAnimations()
    {
        std::vector<Animation*> result{};
        for (const auto& animation: animations | std::views::values)
        {
            result.push_back(animation.get());
        }
        return result;
    }

    bool Sprite::IsMouseWithin(Camera* camera)
    {
        if (camera == nullptr)
            camera = Camera::main;

        AnimationFrame* frame = GetCurrentFrame();
        if (!frame)
            return false;

        float spriteW = frame->atlasW * scaleX;
        float spriteH = frame->atlasH * scaleY;

        float boundsLowerX;
        float boundsLowerY;
        float boundsUpperX;
        float boundsUpperY;

        if (centered)
        {
            float halfW = spriteW * 0.5f;
            float halfH = spriteH * 0.5f;

            boundsLowerX = position.x - halfW;
            boundsLowerY = position.y - halfH;
            boundsUpperX = position.x + halfW;
            boundsUpperY = position.y + halfH;
        }
        else
        {
            boundsLowerX = position.x;
            boundsLowerY = position.y;
            boundsUpperX = position.x + spriteW;
            boundsUpperY = position.y + spriteH;
        }

        Vec2<float> mousePosition;
        if (camera)
            mousePosition = camera->GetMouseGlobalPosition();
        else
            mousePosition = Input::GetMouseScreenPosition();

        return (mousePosition.x > boundsLowerX && mousePosition.x < boundsUpperX) && (mousePosition.y > boundsLowerY && mousePosition.y < boundsUpperY);
    }
}