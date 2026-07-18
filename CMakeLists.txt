cmake_minimum_required(VERSION 3.30)
project(igneous VERSION 0.1.0 LANGUAGES CXX)

if (POLICY CMP0219)
    cmake_policy(SET CMP0219 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# When embedded in a game project, skip examples/tests unless explicitly enabled.
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(_IGNEOUS_TOP_LEVEL ON)
else()
    set(_IGNEOUS_TOP_LEVEL OFF)
endif()

if (_IGNEOUS_TOP_LEVEL)
    option(IGNEOUS_BUILD_EXAMPLES "Build engine feature examples" ON)
    option(IGNEOUS_BUILD_TESTS "Build engine unit tests" ON)
else()
    option(IGNEOUS_BUILD_EXAMPLES "Build engine feature examples" OFF)
    option(IGNEOUS_BUILD_TESTS "Build engine unit tests" OFF)
endif()

set(UPNPC_BUILD_STATIC    ON  CACHE BOOL "" FORCE)
set(UPNPC_BUILD_SHARED    OFF CACHE BOOL "" FORCE)
set(UPNPC_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(UPNPC_BUILD_SAMPLE    OFF CACHE BOOL "" FORCE)
set(UPNPC_NO_INSTALL      ON  CACHE BOOL "" FORCE)

# --- Dependencies ---

add_subdirectory(libs/miniupnp/miniupnpc)
add_subdirectory(libs/SDL3)
add_subdirectory(libs/SDL3_image)
add_subdirectory(libs/SQLiteCpp)
add_subdirectory(libs/SDL3_mixer)
add_subdirectory(libs/enet)
set(_igneous_saved_build_testing ${BUILD_TESTING})
set(BUILD_TESTING OFF)
add_subdirectory(libs/pfr)
set(BUILD_TESTING ${_igneous_saved_build_testing})

# --- ImGui ---

set(IMGUI_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_tables.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_widgets.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_sdl3.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends
)

target_link_libraries(imgui PRIVATE SDL3::SDL3)

# --- Engine ---

file(GLOB_RECURSE ENGINE_SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADER_FILES "include/igneous/*.hpp")

add_library(igneous STATIC
        ${ENGINE_SOURCE_FILES}
        ${ENGINE_HEADER_FILES}
)

target_include_directories(igneous PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/enet/include
)

target_link_libraries(igneous PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_mixer::SDL3_mixer
        SQLiteCpp
        imgui
        enet
        boost_pfr
        libminiupnpc-static
)

# --- Steamworks (optional) ---
# Must come after add_library(igneous) so the target exists before we modify it.
# IGNEOUS_STEAM_DLL is cached as INTERNAL so IgneousHelpers.cmake can read it in the game's scope.

option(IGNEOUS_STEAM "Enable Steamworks integration" OFF)

if (IGNEOUS_STEAM)
    set(STEAMWORKS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/steamworks)

    if (WIN32)
        set(STEAM_LIB "${STEAMWORKS_DIR}/redistributable_bin/win64/steam_api64.dll")
    elseif (UNIX AND NOT APPLE)
        set(STEAM_LIB "${STEAMWORKS_DIR}/redistributable_bin/linux64/libsteam_api.so")
    elseif (APPLE)
        set(STEAM_LIB "${STEAMWORKS_DIR}/redistributable_bin/osx/libsteam_api.dylib")
    endif()

    if (NOT EXISTS "${STEAM_LIB}")
        message(FATAL_ERROR
                "IGNEOUS_STEAM is ON but Steamworks SDK binaries not found at ${STEAM_LIB}. "
                "See libs/steamworks/README.md for setup instructions."
        )
    endif()

    # Cache the resolved DLL/so path so IgneousHelpers.cmake can copy it directly
    # without relying on the steamworks target being visible in the game's scope
    set(IGNEOUS_STEAM_DLL "${STEAM_LIB}" CACHE INTERNAL "Path to the Steamworks shared library")

    add_library(steamworks SHARED IMPORTED GLOBAL)
    target_include_directories(steamworks INTERFACE ${STEAMWORKS_DIR}/public)

    if (WIN32)
        set_target_properties(steamworks PROPERTIES
                IMPORTED_LOCATION "${STEAMWORKS_DIR}/redistributable_bin/win64/steam_api64.dll"
                IMPORTED_IMPLIB   "${STEAMWORKS_DIR}/redistributable_bin/win64/steam_api64.lib"
        )
    elseif (UNIX AND NOT APPLE)
        set_target_properties(steamworks PROPERTIES
                IMPORTED_LOCATION "${STEAMWORKS_DIR}/redistributable_bin/linux64/libsteam_api.so"
        )
    elseif (APPLE)
        set_target_properties(steamworks PROPERTIES
                IMPORTED_LOCATION "${STEAMWORKS_DIR}/redistributable_bin/osx/libsteam_api.dylib"
        )
    endif()

    target_link_libraries(igneous PUBLIC steamworks)
    target_compile_definitions(igneous PUBLIC IGNEOUS_STEAM_ENABLED)

    message(STATUS "Igneous: Steamworks integration ENABLED")
else()
    message(STATUS "Igneous: Steamworks integration DISABLED")
endif()

add_library(igneous::igneous ALIAS igneous)

# --- Helpers ---
# Exposes igneous_copy_runtime_dependencies() to any project that add_subdirectory's this engine

include(cmake/IgneousHelpers.cmake)

# --- Install (optional SDK prefix) ---
# Ships the static library, headers, and runtime helper module.
# Games should still prefer add_subdirectory / igneous_fetch_content (see docs/linking.md)
# so all vendored dependencies (SDL3, etc.) are built together.

install(
        TARGETS igneous
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/igneous
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
        FILES
        cmake/Igneous.cmake
        cmake/IgneousHelpers.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/igneous
)

# --- Examples ---

include(cmake/IgneousExamples.cmake)

if (IGNEOUS_BUILD_TESTS)
    enable_testing()
    target_compile_definitions(igneous PUBLIC IGNEOUS_BUILD_TESTS)
endif()

if (IGNEOUS_BUILD_EXAMPLES)
    set(IGNEOUS_EXAMPLES
            hello
            static_sprite
            animation
            audio
            camera
            input_keyboard
            input_mouse
            input_gamepad
            input_layers
            input_actions
            scenes
            z_index
            cfg
            database
            events
            perlin
            networking_local
            networking_enet
            networking_steam
            identity
            thread_pool
            renderer
            snake
            pong
    )

    foreach(EXAMPLE IN LISTS IGNEOUS_EXAMPLES)
        igneous_add_example(${EXAMPLE})
    endforeach()
endif()

add_subdirectory(tests)