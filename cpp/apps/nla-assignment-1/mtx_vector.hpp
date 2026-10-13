#pragma once

#include <iomanip>
#include <iostream>
#include <string>

#include <Eigen/Dense>

namespace hpc {

// Reads a MatrixMarket "vector coordinate" file as written by LIS.
// Eigen's loadMarketVector cannot parse it: LIS prefixes every value with
// its (1-based) index.
inline Eigen::VectorXd read_mtx_vector_lis(const char *path) {
  std::ifstream in(path);
  if (!in) {
    std::cerr << "Could not open " << path << "\n";
  }

  std::string header;
  std::getline(in, header);
  if (header.find("vector coordinate") == std::string::npos) {
    std::cerr << "File has an invalid format: " << path << "\n";
  }

  Eigen::Index size = 0;
  in >> size;

  Eigen::VectorXd vec = Eigen::VectorXd::Zero(size);
  Eigen::Index index = 0;
  double value = 0.0;
  while (in >> index >> value) {
    vec(index - 1) = value;
  }

  return vec;
}

inline void write_mtx_vector_lis(const Eigen::VectorXd &vec, const char *path) {
  std::ofstream out(path);
  if (!out) {
    std::cerr << "Could not open " << path << "\n";
  }

  out << "%%MatrixMarket vector coordinate real general\n";
  out << vec.size() << "\n";
  out << std::scientific << std::setprecision(17);

  for (Eigen::Index i = 0; i < vec.size(); ++i) {
    out << i + 1 << " " << vec(i) << "\n";
  }
}

} // namespace hpc
