include_guard(GLOBAL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined;-fno-omit-frame-pointer;-fno-sanitize-recover=all>")
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # clang statically links both runtimes with a single flag; GCC needs per-runtime flags.
    add_link_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined;-static-libsan>")
  else()
    add_link_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined;-static-libasan;-static-libubsan>")
  endif()
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT lto_output)
if(lto_supported)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
else()
  message(STATUS "LTO not supported: ${lto_output}")
endif()
