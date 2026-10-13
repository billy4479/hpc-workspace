#pragma once

#include <tuple>

#include <Eigen/Dense>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>

namespace hpc {

// Solves A x = b for SPD A with Conjugate Gradient.
// Returns <solution, iteration_count, relative residual>.
inline std::tuple<Eigen::VectorXd, int, double>
solve_eigen_spd(const Eigen::SparseMatrix<double> &A, const Eigen::VectorXd &b,
                double tol) {
  Eigen::ConjugateGradient<Eigen::SparseMatrix<double>,
                           Eigen::Lower | Eigen::Upper>
      solver;
  solver.setTolerance(tol);
  solver.compute(A);
  Eigen::VectorXd x = solver.solve(b);

  return {x, static_cast<int>(solver.iterations()), solver.error()};
}

// Solves A x = b with BiCGSTAB.
// Returns <solution, iteration_count, relative residual>.
inline std::tuple<Eigen::VectorXd, int, double>
solve_eigen_bicgstab(const Eigen::SparseMatrix<double> &A,
                     const Eigen::VectorXd &b, double tol) {
  Eigen::BiCGSTAB<Eigen::SparseMatrix<double, Eigen::RowMajor>> solver;
  solver.setTolerance(tol);
  solver.compute(A);
  Eigen::VectorXd x = solver.solve(b);

  return {x, static_cast<int>(solver.iterations()), solver.error()};
}

// Solves A x = b with a direct LDL^T factorization. Only valid for symmetric
// A: it reads just the lower triangle and assumes the upper one mirrors it.
// Returns <solution, iteration_count (always 0, direct method), relative
// residual>.
inline std::tuple<Eigen::VectorXd, int, double>
solve_eigen_ldlt(const Eigen::SparseMatrix<double> &A,
                 const Eigen::VectorXd &b) {
  Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
  solver.compute(A);
  Eigen::VectorXd x = solver.solve(b);

  return {x, 0, (b - A * x).norm() / b.norm()};
}

} // namespace hpc
