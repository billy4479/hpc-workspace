include_guard(GLOBAL)

# Adds an executable from the app folder cpp/apps/<name>/.
# Source paths are relative to that folder, e.g. add_experiment(intro main.cpp).
# Declare dependencies separately with target_link_libraries.
function(add_experiment name)
  foreach(src IN LISTS ARGN)
    list(APPEND sources ${CMAKE_SOURCE_DIR}/cpp/apps/${name}/${src})
  endforeach()
  add_executable(${name} ${sources})
  install(TARGETS ${name})
endfunction()
