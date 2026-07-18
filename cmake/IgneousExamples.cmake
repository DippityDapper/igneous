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

    if (EXISTS ${EXAMPLE_DIR}/assets)
        add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${EXAMPLE_DIR}/assets
                $<TARGET_FILE_DIR:${TARGET_NAME}>/assets
                VERBATIM
        )
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/${EXAMPLE_NAME}
            VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/examples/${EXAMPLE_NAME}
    )
endfunction()
