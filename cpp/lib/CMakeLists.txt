add_library(hpc_headers INTERFACE)
add_library(hpc::headers ALIAS hpc_headers)
target_include_directories(hpc_headers INTERFACE include)

add_library(hpc_stb STATIC src/stb.cpp)
add_library(hpc::stb ALIAS hpc_stb)
target_link_libraries(hpc_stb PUBLIC hpc::headers PkgConfig::STB)
