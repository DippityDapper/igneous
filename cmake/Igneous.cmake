# Igneous.cmake — consumer entry point for game projects.
#
# Typical usage (submodule at external/igneous):
#
#   include(external/igneous/cmake/Igneous.cmake)
#   igneous_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/igneous)
#   igneous_add_executable(my_game src/main.cpp)
#
# Or fetch a tagged release:
#
#   include(FetchContent)
#   include(${igneous_SOURCE_DIR}/cmake/Igneous.cmake)  # after fetch — see igneous_fetch_content
#
include_guard(GLOBAL)

set(_IGNEOUS_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")

#[=======================================================================[.rst:
igneous_add_subdirectory
------------------------

Add Igneous as a CMake subproject (recommended integration path).

  igneous_add_subdirectory(<source-dir>
      [BINARY_DIR <dir>]
      [BUILD_EXAMPLES]
      [BUILD_TESTS]
      [STEAM ON|OFF]
  )

When embedded in a game, examples and tests default to OFF unless explicitly
requested.
#]=======================================================================]
function(igneous_add_subdirectory IGNEOUS_SOURCE_DIR)
    set(options BUILD_EXAMPLES BUILD_TESTS)
    set(oneValueArgs BINARY_DIR STEAM)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN})

    if (NOT IS_ABSOLUTE "${IGNEOUS_SOURCE_DIR}")
        set(IGNEOUS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${IGNEOUS_SOURCE_DIR}")
    endif()

    if (NOT EXISTS "${IGNEOUS_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "igneous_add_subdirectory: ${IGNEOUS_SOURCE_DIR} is not an Igneous source tree")
    endif()

    if (ARG_BINARY_DIR)
        if (NOT IS_ABSOLUTE "${ARG_BINARY_DIR}")
            set(_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/${ARG_BINARY_DIR}")
        else()
            set(_binary_dir "${ARG_BINARY_DIR}")
        endif()
    else()
        set(_binary_dir "${CMAKE_BINARY_DIR}/_deps/igneous-build")
    endif()

    if (NOT ARG_BUILD_EXAMPLES)
        set(IGNEOUS_BUILD_EXAMPLES OFF CACHE BOOL "Build engine feature examples" FORCE)
    endif()

    if (NOT ARG_BUILD_TESTS)
        set(IGNEOUS_BUILD_TESTS OFF CACHE BOOL "Build engine unit tests" FORCE)
    endif()

    if (ARG_STEAM)
        set(IGNEOUS_STEAM "${ARG_STEAM}" CACHE BOOL "Enable Steamworks integration" FORCE)
    endif()

    add_subdirectory("${IGNEOUS_SOURCE_DIR}" "${_binary_dir}")

    set(IGNEOUS_ROOT "${IGNEOUS_SOURCE_DIR}" CACHE INTERNAL "Igneous source root")
    set(IGNEOUS_BINARY_DIR "${_binary_dir}" CACHE INTERNAL "Igneous build directory")

    include("${IGNEOUS_SOURCE_DIR}/cmake/IgneousHelpers.cmake")
endfunction()

#[=======================================================================[.rst:
igneous_fetch_content
---------------------

Download Igneous with FetchContent (submodules included).

  igneous_fetch_content(
      GIT_REPOSITORY <url>
      [GIT_TAG <tag>]
  )

After this call, ``igneous_add_executable`` works the same as with
``igneous_add_subdirectory``.
#]=======================================================================]
function(igneous_fetch_content)
    set(oneValueArgs GIT_REPOSITORY GIT_TAG)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

    if (NOT ARG_GIT_REPOSITORY)
        message(FATAL_ERROR "igneous_fetch_content requires GIT_REPOSITORY")
    endif()

    if (NOT ARG_GIT_TAG)
        set(ARG_GIT_TAG "main")
    endif()

    include(FetchContent)

    set(IGNEOUS_BUILD_EXAMPLES OFF CACHE BOOL "Build engine feature examples" FORCE)
    set(IGNEOUS_BUILD_TESTS OFF CACHE BOOL "Build engine unit tests" FORCE)

    FetchContent_Declare(
            igneous
            GIT_REPOSITORY "${ARG_GIT_REPOSITORY}"
            GIT_TAG "${ARG_GIT_TAG}"
            GIT_SUBMODULES_RECURSE TRUE
    )

    FetchContent_MakeAvailable(igneous)

    set(IGNEOUS_ROOT "${igneous_SOURCE_DIR}" CACHE INTERNAL "Igneous source root")
    set(IGNEOUS_BINARY_DIR "${igneous_BINARY_DIR}" CACHE INTERNAL "Igneous build directory")

    include("${igneous_SOURCE_DIR}/cmake/IgneousHelpers.cmake")
endfunction()

function(_igneous_require_root)
    if (NOT IGNEOUS_ROOT OR NOT TARGET igneous)
        message(FATAL_ERROR
                "Igneous is not available. Call igneous_add_subdirectory() or "
                "igneous_fetch_content() before linking a game target."
        )
    endif()
endfunction()

#[=======================================================================[.rst:
igneous_link_target
-------------------

Link an existing executable to Igneous and copy runtime shared libraries.

  igneous_link_target(<target> [ASSETS <assets-dir>])
#]=======================================================================]
function(igneous_link_target TARGET_NAME)
    set(oneValueArgs ASSETS)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "" ${ARGN})

    _igneous_require_root()

    if (NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "igneous_link_target: target '${TARGET_NAME}' does not exist")
    endif()

    target_link_libraries(${TARGET_NAME} PRIVATE igneous::igneous)

    include("${IGNEOUS_ROOT}/cmake/IgneousHelpers.cmake")
    igneous_copy_runtime_dependencies(${TARGET_NAME} "${CMAKE_CURRENT_SOURCE_DIR}")

    if (ARG_ASSETS)
        if (NOT IS_ABSOLUTE "${ARG_ASSETS}")
            set(_assets_dir "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_ASSETS}")
        else()
            set(_assets_dir "${ARG_ASSETS}")
        endif()

        if (EXISTS "${_assets_dir}")
            add_custom_command(
                    TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${_assets_dir}"
                    "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets"
                    VERBATIM
            )
        endif()
    endif()
endfunction()

#[=======================================================================[.rst:
igneous_add_executable
----------------------

Create a game executable, link Igneous, and copy runtime dependencies.

  igneous_add_executable(<target> [sources...]
      [ASSETS <assets-dir>]
  )
#]=======================================================================]
function(igneous_add_executable TARGET_NAME)
    set(oneValueArgs ASSETS)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(ARG "" "${oneValueArgs}" "SOURCES" ${ARGN})

    _igneous_require_root()

    if (ARG_UNPARSED_ARGUMENTS)
        add_executable(${TARGET_NAME} ${ARG_UNPARSED_ARGUMENTS})
    elseif (ARG_SOURCES)
        add_executable(${TARGET_NAME} ${ARG_SOURCES})
    else()
        message(FATAL_ERROR "igneous_add_executable: provide at least one source file")
    endif()

    if (ARG_ASSETS)
        igneous_link_target(${TARGET_NAME} ASSETS "${ARG_ASSETS}")
    else()
        igneous_link_target(${TARGET_NAME})
    endif()
endfunction()
