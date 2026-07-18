#pragma once

#include "TestReset.hpp"

#include <SDL3/SDL.h>

struct SdlFixture
{
    SdlFixture()
    {
        Engine::TestReset::All();
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD);
        Engine::Input::Init();
    }

    ~SdlFixture()
    {
        Engine::Input::ResetForTests();
        SDL_Quit();
        Engine::TestReset::All();
    }
};
