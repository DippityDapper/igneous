cmake_minimum_required(VERSION 3.30)
project(igneous)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---

add_subdirectory(libs/SDL3)
add_subdirectory(libs/SDL3_image)
add_subdirectory(libs/SQLiteCpp)
add_subdirectory(libs/SDL3_mixer)
add_subdirectory(libs/enet)

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

# --- Engine Files ---

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
)

function(igneous_copy_runtime_dependencies TARGET_NAME)
    if (WIN32)
        get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)

        add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_mixer::SDL3_mixer>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:enet>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SQLiteCpp>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SQLite::SQLite3>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"

                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINGW_BIN_DIR}/libwinpthread-1.dll" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINGW_BIN_DIR}/libstdc++-6.dll" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                VERBATIM
        )
    endif()
endfunction()