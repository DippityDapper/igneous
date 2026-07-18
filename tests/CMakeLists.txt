if (NOT IGNEOUS_BUILD_TESTS)
    return()
endif()

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
)
FetchContent_MakeAvailable(Catch2)

set(IGNEOUS_TEST_SOURCES
        engine_test.cpp
        input_test.cpp
        rendering_test.cpp
        scenes_test.cpp
        resources_test.cpp
        networking_test.cpp
        test_reset_listener.cpp
)

add_executable(igneous_tests ${IGNEOUS_TEST_SOURCES})

target_link_libraries(igneous_tests PRIVATE igneous Catch2::Catch2WithMain)

target_include_directories(igneous_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(igneous_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

enable_testing()

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(igneous_tests)
