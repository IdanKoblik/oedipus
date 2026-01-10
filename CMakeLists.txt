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

find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)

set(PROTO_FILES
        proto/editor.proto
)

set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

protobuf_generate_cpp(
        GENERATED_PROTO_SRCS
        GENERATED_PROTO_HDRS
        ${PROTO_FILES}
)

set_source_files_properties(
        ${GENERATED_PROTO_SRCS}
        ${GENERATED_PROTO_HDRS}
        PROPERTIES GENERATED TRUE
)

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
        src/net/*.cpp
        include/*.h
)

file(GLOB_RECURSE TEST_SOURCES
        CONFIGURE_DEPENDS
        tests/*.cpp
)

add_executable(oedipus
        ${SOURCES}
        ${GENERATED_PROTO_SRCS}
        ${GENERATED_PROTO_HDRS}
)

target_include_directories(oedipus PRIVATE
        include
        ${PROTO_GEN_DIR}
)

target_link_libraries(oedipus PRIVATE
        protobuf::libprotobuf
        absl::base
        absl::strings
        absl::log
        absl::log_internal_check_op
)

add_executable(oedipus_tests
        ${TEST_SOURCES}
        ${LIB_SOURCES}
        ${GENERATED_PROTO_SRCS}
        ${GENERATED_PROTO_HDRS}
)

target_include_directories(oedipus_tests PRIVATE
        include
        ${PROTO_GEN_DIR}
)

target_link_libraries(oedipus_tests PRIVATE
        protobuf::libprotobuf
        absl::base
        absl::strings
        absl::log
        absl::log_internal_check_op
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(oedipus_tests)
