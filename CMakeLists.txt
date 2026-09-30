cmake_minimum_required(VERSION 3.25)

project(hpc-workspace VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(STB REQUIRED IMPORTED_TARGET stb)

# LIS ships neither pkg-config nor CMake config files, so search manually.
find_path(LIS_INCLUDE_DIR NAMES lis.h)
find_library(LIS_LIBRARY NAMES lis)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIS DEFAULT_MSG LIS_LIBRARY LIS_INCLUDE_DIR)
if(NOT TARGET LIS::LIS)
  add_library(LIS::LIS UNKNOWN IMPORTED)
  set_target_properties(LIS::LIS PROPERTIES
    IMPORTED_LOCATION "${LIS_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${LIS_INCLUDE_DIR}")
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined;-fno-omit-frame-pointer;-fno-sanitize-recover=all>")
  add_link_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined;-static-libasan;-static-libubsan>")
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT lto_output)
if(lto_supported)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
else()
  message(STATUS "LTO not supported: ${lto_output}")
endif()

# Adds an executable from cpp/apps/<name>.cpp linked against the hpc lib.
function(add_experiment name)
  add_executable(${name} ${CMAKE_SOURCE_DIR}/cpp/apps/${name}.cpp)
  target_link_libraries(${name} PRIVATE hpc::hpc)
  install(TARGETS ${name})
endfunction()

add_subdirectory(cpp/lib)
add_subdirectory(cpp/apps)
