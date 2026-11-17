add_experiment(intro
  SOURCES main.cpp
  LIBRARIES Eigen3::Eigen hpc::stb)

add_experiment(nla-assignment-1
  SOURCES main.cpp
  LIBRARIES Eigen3::Eigen hpc::stb LIS::LIS)

add_experiment(nla-assignment-2
  SOURCES main.cpp
  LIBRARIES Eigen3::Eigen hpc::stb)

add_experiment(nla-exam
  SOURCES main.cpp
  LIBRARIES Eigen3::Eigen)

add_experiment(amsc-dataset SOURCES main.cpp dataset.cpp)

add_experiment(lis-eigen1 SOURCES etest1.c LIBRARIES LIS::LIS)
add_experiment(lis-eigen5 SOURCES etest5.c LIBRARIES LIS::LIS)
add_experiment(lis-test1 SOURCES test1.c LIBRARIES LIS::LIS)
