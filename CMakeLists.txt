cmake_minimum_required(VERSION 4.0)
project(oedipus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
   CONFIGURE_DEPENDS
   src/*.cpp
   include/*.h
)

add_executable(oedipus ${SOURCES})
target_include_directories(oedipus PRIVATE include)
