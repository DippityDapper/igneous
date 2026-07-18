#pragma once

#include "igneous/engine/CFGParser.hpp"
#include "igneous/engine/Camera.hpp"
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
        Input::ResetForTests();
        Window::ResetForTests();
        Renderer::ResetForTests();
        CFGParser::ResetForTests();
        SceneManager::ResetForTests();
        ResourceManager::ResetForTests();
        Time::ResetForTests();
        Camera::ResetForTests();
    }
}
