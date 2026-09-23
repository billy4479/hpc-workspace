cmake_minimum_required(VERSION 3.25)

project(hpc-workspace VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(STB REQUIRED IMPORTED_TARGET stb)

add_compile_options(-Wall -Wextra -Wpedantic)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined;-fno-omit-frame-pointer;-fno-sanitize-recover=all>")
  add_link_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined;-static-libasan;-static-libubsan>")
endif()

# Adds an executable from cpp/apps/<name>.cpp linked against the hpc lib.
function(add_experiment name)
  add_executable(${name} ${CMAKE_SOURCE_DIR}/cpp/apps/${name}.cpp)
  target_link_libraries(${name} PRIVATE hpc::hpc)
  install(TARGETS ${name})
endfunction()

add_subdirectory(cpp/lib)
add_subdirectory(cpp/apps)
