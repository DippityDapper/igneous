#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "igneous/engine/Vec2.hpp"
#include "igneous/resources/Animation.hpp"
#include "igneous/resources/AnimationFrame.hpp"
#include "igneous/resources/AudioStream.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/resources/Sprite.hpp"

#include "igneous/rendering/Renderer.hpp"
#include "igneous/rendering/Window.hpp"

#include "SDL3/SDL.h"
#include "SDL3/SDL_render.h"
#include "SDL3_mixer/SDL_mixer.h"

using Catch::Matchers::WithinAbs;

struct RenderResourceFixture
{
    RenderResourceFixture()
    {
        REQUIRE(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO));
        Engine::Window::Init(640, 360);
        Engine::Renderer::Init();
    }

    ~RenderResourceFixture()
    {
        Engine::ResourceManager::ResetForTests();
        Engine::Renderer::Clean();
        Engine::Window::Clean();
        SDL_Quit();
    }
};

struct AudioTestFixture
{
    AudioTestFixture()
    {
        REQUIRE(SDL_Init(SDL_INIT_AUDIO));
        REQUIRE(MIX_Init());
    }

    ~AudioTestFixture()
    {
        Engine::ResourceManager::ResetForTests();
        MIX_Quit();
        SDL_Quit();
    }
};

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

TEST_CASE("Sprite rotation copies with copy constructor", "[resources][sprite]")
{
    Engine::Vec2<float> position{0.0f, 0.0f};
    Engine::Sprite original(position);
    original.rotation = 90.0f;

    Engine::Sprite copy(original);
    REQUIRE(copy.rotation == 90.0f);
}

TEST_CASE("ResourceManager assigns monotonic sprite IDs", "[resources][resourcemanager]")
{
    Engine::ResourceManager::ResetForTests();

    Engine::Vec2<float> pos1{0.0f, 0.0f};
    Engine::Vec2<float> pos2{10.0f, 10.0f};
    Engine::Vec2<float> pos3{20.0f, 20.0f};

    Engine::Sprite sprite1(pos1);
    Engine::Sprite sprite2(pos2);
    Engine::Sprite sprite3(pos3);

    REQUIRE(sprite1.id != 0);
    REQUIRE(sprite2.id != 0);
    REQUIRE(sprite3.id != 0);
    REQUIRE(sprite1.id < sprite2.id);
    REQUIRE(sprite2.id < sprite3.id);

    const int unregisteredId = sprite2.id;
    REQUIRE(Engine::ResourceManager::UnregisterSprite(unregisteredId));

    Engine::Vec2<float> pos4{30.0f, 30.0f};
    Engine::Sprite sprite4(pos4);
    REQUIRE(sprite4.id > sprite3.id);

    REQUIRE(Engine::ResourceManager::RegisterSprite(&sprite2));
    REQUIRE(sprite2.id > sprite4.id);
}

TEST_CASE("ResourceManager scale mode", "[resources][resourcemanager]")
{
    Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_NEAREST);
    Engine::ResourceManager::SetScaleMode(SDL_SCALEMODE_LINEAR);
    SUCCEED();
}

TEST_CASE("ResourceManager registers sprites and updates z-index", "[resources][resourcemanager]")
{
    Engine::Vec2<float> position{0.0f, 0.0f};
    Engine::Sprite sprite(position);

    REQUIRE(sprite.id != 0);
    REQUIRE_FALSE(Engine::ResourceManager::RegisterSprite(&sprite));

    sprite.SetZIndex(1);
    sprite.SetZIndex(5);
    REQUIRE(sprite.GetZIndex() == 5);

    REQUIRE(Engine::ResourceManager::UnregisterSprite(sprite.id));
    REQUIRE_FALSE(Engine::ResourceManager::UnregisterSprite(sprite.id));
}

TEST_CASE("ResourceManager loads and caches textures", "[resources][resourcemanager]")
{
    RenderResourceFixture render;
    (void) render;

    std::shared_ptr<SDL_Texture> first = Engine::ResourceManager::LoadTexture(IGNEOUS_TEST_TEXTURE_PNG);
    REQUIRE(first != nullptr);

    std::shared_ptr<SDL_Texture> second = Engine::ResourceManager::LoadTexture(IGNEOUS_TEST_TEXTURE_PNG);
    REQUIRE(second != nullptr);
    REQUIRE(first.get() == second.get());
}

TEST_CASE("ResourceManager loads sound and plays through AudioStream", "[resources][resourcemanager]")
{
    AudioTestFixture audio;
    (void) audio;

    std::shared_ptr<Engine::AudioStream> sound = Engine::ResourceManager::LoadSound(IGNEOUS_TEST_AUDIO_WAV, 0);
    REQUIRE(sound != nullptr);

    std::shared_ptr<Engine::AudioStream> cached = Engine::ResourceManager::LoadSound(IGNEOUS_TEST_AUDIO_WAV, 0);
    REQUIRE(cached.get() == sound.get());

    sound->Play(0.5f);
    REQUIRE(Engine::ResourceManager::GetMixer() != nullptr);
}

TEST_CASE("AudioStream Play without prior LoadSound", "[resources][audiostream]")
{
    AudioTestFixture audio;
    (void) audio;

    MIX_Audio* rawStream = MIX_LoadAudio(nullptr, IGNEOUS_TEST_AUDIO_WAV, true);
    REQUIRE(rawStream != nullptr);

    REQUIRE(Engine::ResourceManager::tracks.empty());

    Engine::AudioStream stream(rawStream, 0);
    stream.Play(1.0f);

    REQUIRE_FALSE(Engine::ResourceManager::tracks.empty());
    REQUIRE(Engine::ResourceManager::GetMixer() != nullptr);
}

TEST_CASE("AudioStream AcquireAudioTrack steals oldest when all are busy", "[resources][audiostream]")
{
    AudioTestFixture audio;
    (void) audio;

    REQUIRE(Engine::ResourceManager::EnsureAudioTracks());

    for (auto& trackState: Engine::ResourceManager::tracks)
        trackState.second = false;

    MIX_Track* stolen = Engine::ResourceManager::AcquireAudioTrack();
    REQUIRE(stolen == Engine::ResourceManager::tracks.begin()->first);
    REQUIRE(Engine::ResourceManager::tracks[stolen] == false);
}

TEST_CASE("AudioStream Play returns safely when audio is unavailable", "[resources][audiostream]")
{
    Engine::AudioStream stream(nullptr, 0);
    stream.Play(1.0f);
    SUCCEED();
}
