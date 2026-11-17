include_guard(GLOBAL)

# Opt in per program with target_link_libraries(<name> PRIVATE OpenMP::OpenMP_CXX)
# or OpenMP::OpenMP_C for C programs.
find_package(OpenMP REQUIRED COMPONENTS C CXX)
