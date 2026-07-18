cmake_minimum_required(VERSION 3.30)
project(my_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Igneous as a git submodule at external/igneous (see README.md).
include(${CMAKE_CURRENT_SOURCE_DIR}/external/igneous/cmake/Igneous.cmake)
igneous_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/igneous)

igneous_add_executable(my_game
    src/main.cpp
)

# Optional: copy assets/ next to the executable at build time
# igneous_add_executable(my_game src/main.cpp ASSETS assets)
