include_guard(GLOBAL)

# Adds an executable from the app folder cpp/apps/<name>/ linked against the hpc lib.
# Source paths are relative to that folder, e.g. add_experiment(intro main.cpp)
function(add_experiment name)
  foreach(src IN LISTS ARGN)
    list(APPEND sources ${CMAKE_SOURCE_DIR}/cpp/apps/${name}/${src})
  endforeach()
  add_executable(${name} ${sources})
  target_link_libraries(${name} PRIVATE hpc::hpc)
  install(TARGETS ${name})
endfunction()
