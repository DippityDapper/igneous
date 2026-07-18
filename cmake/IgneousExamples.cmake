# IgneousExamples.cmake — register standalone engine feature examples.

function(igneous_add_example EXAMPLE_NAME)
    set(EXAMPLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples/${EXAMPLE_NAME})

    if (NOT EXISTS ${EXAMPLE_DIR}/main.cpp)
        message(FATAL_ERROR "igneous_add_example: missing ${EXAMPLE_DIR}/main.cpp")
    endif()

    set(TARGET_NAME example_${EXAMPLE_NAME})
    add_executable(${TARGET_NAME} ${EXAMPLE_DIR}/main.cpp)
    target_link_libraries(${TARGET_NAME} PRIVATE igneous)
    igneous_copy_runtime_dependencies(${TARGET_NAME} ${EXAMPLE_DIR})

    set(EXAMPLE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/examples/${EXAMPLE_NAME})
    set(ASSETS_SRC ${EXAMPLE_DIR}/assets)
    set(ASSETS_DST ${EXAMPLE_OUTPUT_DIR}/assets)

    if (IS_DIRECTORY ${ASSETS_SRC})
        # Sync on configure so new assets appear without rebuilding main.cpp.
        file(MAKE_DIRECTORY ${ASSETS_DST})
        file(COPY ${ASSETS_SRC}/ DESTINATION ${ASSETS_DST})

        file(GLOB_RECURSE _example_asset_files CONFIGURE_DEPENDS
                LIST_DIRECTORIES false
                "${ASSETS_SRC}/*")

        set(_assets_stamp ${EXAMPLE_OUTPUT_DIR}/.assets.stamp)
        add_custom_command(
                OUTPUT ${_assets_stamp}
                COMMAND ${CMAKE_COMMAND} -E rm -rf ${ASSETS_DST}
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SRC} ${ASSETS_DST}
                COMMAND ${CMAKE_COMMAND} -E touch ${_assets_stamp}
                DEPENDS ${_example_asset_files}
                COMMENT "Syncing ${EXAMPLE_NAME} example assets"
                VERBATIM
        )

        add_custom_target(${TARGET_NAME}_assets DEPENDS ${_assets_stamp})
        add_dependencies(${TARGET_NAME} ${TARGET_NAME}_assets)
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_OUTPUT_DIR}
            VS_DEBUGGER_WORKING_DIRECTORY ${EXAMPLE_OUTPUT_DIR}
    )
endfunction()
