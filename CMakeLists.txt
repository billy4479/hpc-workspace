cmake_minimum_required(VERSION 3.25)

project(hpc-workspace VERSION 0.1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(compile-options)
include(dependencies/eigen3)
include(dependencies/stb)
include(dependencies/lis)
include(add-experiment)

add_subdirectory(cpp/lib)
add_subdirectory(cpp/apps)
