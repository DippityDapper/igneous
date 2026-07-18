#include <catch2/catch_test_macros.hpp>

#include "SdlFixture.hpp"
#include "igneous/rendering/Renderer.hpp"
#include "igneous/rendering/Window.hpp"

TEST_CASE("Window stores viewport dimensions", "[rendering][window]")
{
    SdlFixture sdl;
    (void) sdl;

    Engine::Window::Init(800, 600);
    REQUIRE(Engine::Window::viewport.x == 800);
    REQUIRE(Engine::Window::viewport.y == 600);
    REQUIRE(Engine::Window::GetWindow() != nullptr);

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
