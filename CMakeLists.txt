cmake_minimum_required(VERSION 3.25)

project(hpc-workspace VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(hpc-workspace src/main.cpp)

target_compile_options(hpc-workspace PRIVATE -Wall -Wextra -Wpedantic)

install(TARGETS hpc-workspace)
