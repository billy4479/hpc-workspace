add_experiment(intro main.cpp)
target_link_libraries(intro PRIVATE Eigen3::Eigen hpc::stb)

add_experiment(nla-assignment-1 main.cpp)
target_link_libraries(nla-assignment-1 PRIVATE Eigen3::Eigen hpc::stb LIS::LIS)

add_experiment(nla-assignment-2 main.cpp)
target_link_libraries(nla-assignment-2 PRIVATE Eigen3::Eigen hpc::stb)

add_experiment(amsc-dataset main.cpp dataset.cpp)

add_experiment(lis-eigen1 etest1.c)
target_link_libraries(lis-eigen1 PRIVATE LIS::LIS)

add_experiment(lis-eigen5 etest5.c)
target_link_libraries(lis-eigen5 PRIVATE LIS::LIS)

add_experiment(lis-test1 test1.c)
target_link_libraries(lis-test1 PRIVATE LIS::LIS)
