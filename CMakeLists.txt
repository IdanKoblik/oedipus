cmake_minimum_required(VERSION 3.20)
project(oedipus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)

set(PROTO_FILES
        ${CMAKE_SOURCE_DIR}/proto/editor.proto
)

set(PROTO_GEN_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

add_library(editor_proto)

protobuf_generate(
        TARGET editor_proto
        PROTOS ${PROTO_FILES}
        PROTOC_OUT_DIR ${PROTO_GEN_DIR}
)

target_include_directories(editor_proto
        PUBLIC
        ${PROTO_GEN_DIR}
)

target_link_libraries(editor_proto
        PUBLIC
        protobuf::libprotobuf
)

file(GLOB_RECURSE CORE_SOURCES
        CONFIGURE_DEPENDS
        src/*.cpp
)

list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_library(oedipus_core
        ${CORE_SOURCES}
)

target_include_directories(oedipus_core
        PUBLIC
        include
        ${PROTO_GEN_DIR}
)

target_link_libraries(oedipus_core
        PUBLIC
        editor_proto
        absl::base
        absl::strings
        absl::log
        absl::log_internal_check_op
)

add_executable(oedipus
        src/main.cpp
)

target_link_libraries(oedipus
        PRIVATE
        oedipus_core
)