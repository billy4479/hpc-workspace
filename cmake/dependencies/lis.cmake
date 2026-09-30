include_guard(GLOBAL)

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
