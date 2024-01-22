cmake_minimum_required(VERSION 3.23)

project(cpp-quiz VERSION 0.1 LANGUAGES CXX)

include(cmake/util.cmake)
include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# default to release
set(CMAKE_BUILD_TYPE Release)

if (BUILD_LEVEL STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)
    add_definitions(-DDEBUG)
endif()

green_message("Build Level: ${CMAKE_BUILD_TYPE}")

add_subdirectory(answer)
