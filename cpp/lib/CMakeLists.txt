add_library(hpc STATIC src/stb.cpp)
add_library(hpc::hpc ALIAS hpc)

target_include_directories(hpc PUBLIC include)
target_link_libraries(hpc PUBLIC Eigen3::Eigen PkgConfig::STB)
