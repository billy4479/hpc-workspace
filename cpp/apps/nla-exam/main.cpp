#include "grad.hpp"
#include <Eigen/Eigen>
#include <Eigen/Eigenvalues>
#include <Eigen/Sparse>
#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/IterativeLinearSolvers/BasicPreconditioners.h>
#include <Eigen/src/IterativeLinearSolvers/BiCGSTAB.h>
#include <Eigen/src/SparseCore/SparseMatrix.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unsupported/Eigen/src/SparseExtra/MarketIO.h>

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

int main() {
  Eigen::SparseMatrix<double> mat(112, 112);
  for (auto i = 0; i < mat.rows(); i++) {
    mat.insert(i, i) = -8;
    if (i + 1 < mat.cols()) {
      mat.insert(i, i + 1) = 4;
    }
    if (i + 2 < mat.cols()) {
      mat.insert(i, i + 2) = 1;
    }
    if (i - 1 > 0) {
      mat.insert(i, i - 1) = 2;
    }
  }

  std::cout << mat.norm() << "\n";
  Eigen::EigenSolver<Eigen::MatrixXd> solver;
  solver.compute(Eigen::MatrixXd(mat));
  std::cout << solver.eigenvalues() << "\n";

  Eigen::saveMarket(mat, "A_ex2.mtx");

  return 0;
}
