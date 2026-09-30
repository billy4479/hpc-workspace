include_guard(GLOBAL)

# Adds an executable from cpp/apps/<name>.cpp linked against the hpc lib.
function(add_experiment name)
  add_executable(${name} ${CMAKE_SOURCE_DIR}/cpp/apps/${name}.cpp)
  target_link_libraries(${name} PRIVATE hpc::hpc)
  install(TARGETS ${name})
endfunction()
