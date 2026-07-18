#pragma once

#include "igneous/engine/CFGParser.hpp"
#include "igneous/engine/Camera.hpp"
#include "igneous/engine/Engine.hpp"
#include "igneous/engine/Time.hpp"
#include "igneous/input/Input.hpp"
#include "igneous/rendering/Renderer.hpp"
#include "igneous/rendering/Window.hpp"
#include "igneous/resources/ResourceManager.hpp"
#include "igneous/scenes/SceneManager.hpp"

namespace Engine::TestReset
{
    inline void All()
    {
#if defined(IGNEOUS_BUILD_TESTS)
        Engine::Engine::ResetForTests();
#endif
        Input::ResetForTests();
        Input::RestoreBaseline();
        Window::ResetForTests();
        Renderer::ResetForTests();
        CFGParser::ResetForTests();
        SceneManager::ResetForTests();
        ResourceManager::ResetForTests();
        Time::ResetForTests();
        Camera::ResetForTests();
    }
}
