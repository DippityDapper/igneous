#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "igneous/engine/Vec2.hpp"
#include "igneous/resources/Animation.hpp"
#include "igneous/resources/AnimationFrame.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/resources/Sprite.hpp"

#include <SDL3/SDL_render.h>

using Catch::Matchers::WithinAbs;

TEST_CASE("AnimationFrame stores atlas coordinates", "[resources][animationframe]")
{
    Engine::AnimationFrame frame{};
    frame.atlasW = 32;
    frame.atlasH = 64;
    frame.atlasX = 2;
    frame.atlasY = 3;

    REQUIRE(frame.atlasW == 32);
    REQUIRE(frame.atlasH == 64);
    REQUIRE(frame.atlasX == 2);
    REQUIRE(frame.atlasY == 3);
}

TEST_CASE("Animation advances frames by fps", "[resources][animation]")
{
    Engine::Animation animation;
    animation.SetName("walk");
    animation.SetFPS(10.0f);
    animation.AddFrame(std::shared_ptr<SDL_Texture>(nullptr));
    animation.AddFrame(std::shared_ptr<SDL_Texture>(nullptr));

    REQUIRE(animation.GetCurrentFrameIndex() == 0);
    animation.IncrementElapsedTime(0.11);
    REQUIRE(animation.GetCurrentFrameIndex() == 1);
}

TEST_CASE("Sprite manages named animations", "[resources][sprite]")
{
    Engine::Vec2<float> position{0.0f, 0.0f};
    Engine::Sprite sprite(position);

    Engine::Animation* idle = sprite.AddAnimation("idle", true);
    Engine::Animation* walk = sprite.AddAnimation("walk", false);

    REQUIRE(idle != nullptr);
    REQUIRE(walk != nullptr);
    REQUIRE(sprite.GetAnimation("idle") == idle);
    REQUIRE(sprite.GetAnimation("walk") == walk);

    sprite.SetCurrentAnimation("walk", true);
    REQUIRE(sprite.GetCurrentAnimation() == walk);
}

TEST_CASE("ResourceManager scale mode", "[resources][resourcemanager]")
{
    Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_NEAREST);
    Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_LINEAR);
    SUCCEED();
}
