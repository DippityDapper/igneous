#include <catch2/catch_test_macros.hpp>

#include <string>

#include "SdlFixture.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/rendering/Renderer.hpp"
#include "igneous/rendering/Window.hpp"
#include "SDL3/SDL.h"

TEST_CASE("Window stores viewport dimensions", "[rendering][window]")
{
    SdlFixture sdl;
    (void) sdl;

    Engine::Window::Init(800, 600, "Test Window");
    REQUIRE(Engine::Window::viewport.x == 800);
    REQUIRE(Engine::Window::viewport.y == 600);
    REQUIRE(Engine::Window::GetWindow() != nullptr);
    REQUIRE(std::string(SDL_GetWindowTitle(Engine::Window::GetWindow())) == "Test Window");

    Engine::Window::Clean();
}

TEST_CASE("Window resize keeps viewport and flag in sync", "[rendering][window][resize]")
{
    SdlFixture sdl;
    (void) sdl;

    Engine::Window::Init(640, 360);
    Engine::Window::ResetFrameState();

    Engine::Window::OnResize(1024, 768);
    REQUIRE(Engine::Window::viewport.x == 1024);
    REQUIRE(Engine::Window::viewport.y == 768);
    REQUIRE(Engine::Window::WasResized());
    REQUIRE(Engine::Input::IsWindowResized());

    Engine::Window::ResetFrameState();
    REQUIRE_FALSE(Engine::Window::WasResized());
    REQUIRE_FALSE(Engine::Input::IsWindowResized());

    Engine::Window::Clean();
}

TEST_CASE("Renderer initializes with window", "[rendering][renderer]")
{
    SdlFixture sdl;
    (void) sdl;

    Engine::Window::Init(640, 360);
    Engine::Renderer::Init();

    Engine::Renderer::Clean();
    Engine::Window::Clean();
}
