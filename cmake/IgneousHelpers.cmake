# IgneousHelpers.cmake
# Provides igneous_copy_runtime_dependencies(TARGET_NAME)
# Call this in your game's CMakeLists.txt after defining your executable target.

function(igneous_copy_runtime_dependencies TARGET_NAME TARGET_SOURCE_DIR)

    # Copy steam_appid.txt if present — required for Steam to initialize outside the Steam client
    if (IGNEOUS_STEAM AND EXISTS "${TARGET_SOURCE_DIR}/steam_appid.txt")
        add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${TARGET_SOURCE_DIR}/steam_appid.txt"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/steam_appid.txt"
                VERBATIM
        )
    endif()

    if (WIN32)
        get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)

        add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>"             "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_mixer::SDL3_mixer>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:enet>"                   "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SQLiteCpp>"              "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SQLite::SQLite3>"        "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINGW_BIN_DIR}/libwinpthread-1.dll"  "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"  "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINGW_BIN_DIR}/libstdc++-6.dll"      "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                VERBATIM
        )

        # Use IGNEOUS_STEAM_DLL (a plain path) instead of $<TARGET_FILE:steamworks>
        # because imported targets defined in a subdirectory aren't visible in the game's scope
        if (IGNEOUS_STEAM AND IGNEOUS_STEAM_DLL)
            add_custom_command(
                    TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${IGNEOUS_STEAM_DLL}" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                    VERBATIM
            )
        endif()

    elseif (UNIX AND NOT APPLE)
        # Set RPATH so the executable finds .so files sitting next to it at runtime
        set_target_properties(${TARGET_NAME} PROPERTIES
                BUILD_RPATH   "$ORIGIN"
                INSTALL_RPATH "$ORIGIN"
        )

        add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                # enet is static on Linux — no copy needed
                # SQLite3 is statically linked by SQLiteCpp — no copy needed
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>"             "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_mixer::SDL3_mixer>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SQLiteCpp>"              "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                VERBATIM
        )

        if (IGNEOUS_STEAM AND IGNEOUS_STEAM_DLL)
            add_custom_command(
                    TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${IGNEOUS_STEAM_DLL}" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                    VERBATIM
            )
        endif()

    elseif (APPLE)
        # Set RPATH so the executable finds .dylib files sitting next to it at runtime
        set_target_properties(${TARGET_NAME} PROPERTIES
                BUILD_RPATH   "@executable_path"
                INSTALL_RPATH "@executable_path"
        )

        add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>"             "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_mixer::SDL3_mixer>" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SQLiteCpp>"              "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                VERBATIM
        )

        if (IGNEOUS_STEAM AND IGNEOUS_STEAM_DLL)
            add_custom_command(
                    TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${IGNEOUS_STEAM_DLL}" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                    VERBATIM
            )
        endif()

    endif()

endfunction()