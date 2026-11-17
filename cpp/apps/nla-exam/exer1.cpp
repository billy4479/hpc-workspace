#include "grad.hpp"
#include <Eigen/Eigen>
#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/IterativeLinearSolvers/BasicPreconditioners.h>
#include <Eigen/src/IterativeLinearSolvers/BiCGSTAB.h>
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
  Eigen::SparseMatrix<double> mat;
  Eigen::loadMarket(mat, "./Aex1.mtx");

  std::cout << std::scientific << std::setprecision(17);

  const auto is_symmetric = mat.isApprox(mat.transpose());
  const auto norm_mat = mat.norm();
  std::cout << "- Task 1.5: symmetric=" << is_symmetric << ", norm=" << norm_mat
            << "\n";

  Eigen::VectorXd b(mat.rows());
  for (auto i = 0; i < mat.rows(); i++) {
    b(i) = static_cast<double>((i + 1) % 2) * 2;
  }

  Eigen::VectorXd grad_x(b);
  int iter_grad{1000};
  double tol_grad{1e-4};
  const auto grad_success = LinearAlgebra::GRAD(
      mat, grad_x, b, Eigen::DiagonalPreconditioner<double>(mat), iter_grad,
      tol_grad);

  const auto norm_x_grad = grad_x.norm();
  std::cout << "- Task 1.6: iterations=" << iter_grad
            << ", norm=" << norm_x_grad
            << ", success=" << !static_cast<bool>(grad_success)
            << ", tol=" << tol_grad << "\n";

  Eigen::BiCGSTAB<Eigen::SparseMatrix<double>,
                  Eigen::DiagonalPreconditioner<double>>
      bicgstab_solver(mat);
  bicgstab_solver.setTolerance(1e-12);
  const Eigen::VectorXd bicgstab_solution =
      bicgstab_solver.solveWithGuess(b, grad_x);
  const auto norm_diff = (grad_x - bicgstab_solution).norm();
  std::cout << "- Task 1.7: iterations=" << bicgstab_solver.iterations()
            << ", norm_diff=" << norm_diff << "\n";

  write_mtx_vector_lis(b, "b.mtx");

  return 0;
}
