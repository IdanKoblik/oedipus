cmake_minimum_required(VERSION 3.20)
project(oedipus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Enable code coverage" OFF)

enable_testing()

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Building with coverage enabled")

    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE SOURCES
        CONFIGURE_DEPENDS
        src/*.cpp
        include/*.h
)

file(GLOB_RECURSE LIB_SOURCES
        CONFIGURE_DEPENDS
        src/cake.cpp
        src/search.cpp
        src/config/config.cpp
        src/events/event.cpp
        src/listeners/*.cpp
        src/editor.cpp
        src/terminal.cpp
        src/tui/*.cpp
        include/*.h
)

file(GLOB_RECURSE TEST_SOURCES
        CONFIGURE_DEPENDS
        tests/*.cpp
)

add_executable(oedipus ${SOURCES})
target_include_directories(oedipus PRIVATE include)

add_executable(oedipus_tests
        ${TEST_SOURCES}
        ${LIB_SOURCES}
)

target_include_directories(oedipus_tests PRIVATE include)

target_link_libraries(oedipus_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(oedipus_tests)