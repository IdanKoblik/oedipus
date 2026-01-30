cmake_minimum_required(VERSION 3.20)
project(oedipus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Enable code coverage" OFF)

enable_testing()

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

include(FetchContent)

find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)

set(PROTO_FILES ${CMAKE_SOURCE_DIR}/proto/editor.proto)
set(PROTO_GEN_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

add_library(editor_proto)
protobuf_generate(
    TARGET editor_proto
    PROTOS ${PROTO_FILES}
    PROTOC_OUT_DIR ${PROTO_GEN_DIR}
)

target_include_directories(editor_proto PUBLIC ${PROTO_GEN_DIR})
target_link_libraries(editor_proto PUBLIC protobuf::libprotobuf absl::base absl::log absl::log_internal_check_op)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    klogger
    GIT_REPOSITORY https://github.com/IdanKoblik/klogger.git
    GIT_TAG 2026-b1
)
FetchContent_MakeAvailable(klogger)

file(GLOB_RECURSE LIB_SOURCES
    src/structs/*.cpp
    src/config/*.cpp
    src/terminal.cpp
    src/algorithem/*.cpp
    src/tui/*.cpp
    src/utils/*.cpp
    src/net/*.cpp
    src/handlers/*.cpp
    src/editor.cpp
    src/context.cpp
)

add_library(oedipus_lib ${LIB_SOURCES})
target_include_directories(oedipus_lib PUBLIC include ${PROTO_GEN_DIR})
target_link_libraries(oedipus_lib PUBLIC klogger::klogger editor_proto)

add_executable(oedipus src/main.cpp)
target_link_libraries(oedipus PRIVATE oedipus_lib)

file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
add_executable(oedipus_tests ${TEST_SOURCES})
target_link_libraries(oedipus_tests PRIVATE oedipus_lib GTest::gtest GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(oedipus_tests)
