#pragma once

#include <iostream>
#include <sstream>
#include <utility>

#include <lis.h>

namespace hpc {

// Solves the system stored in MatrixMarket files with LIS.
// Returns <iteration_count, residual>. `options` is passed to
// lis_solver_set_option, for example "-i bicgstab -p ilu".
// Serial build only: our LIS is compiled without MPI, no mpirun needed.
inline std::pair<int, double> solve_lis_file_output(const char *matrix_path,
                                                    const char *b_path,
                                                    const char *solution_path,
                                                    const char *options,
                                                    double tol) {
  // Bad workaround to avoid mixing argc/argv, works for this challenge.
  int argc = 1;
  char program_name[] = "nla";
  char *argv_data[] = {program_name, nullptr};
  char **argv = argv_data;
  lis_initialize(&argc, &argv);

  LIS_MATRIX A = nullptr;
  LIS_VECTOR b = nullptr;
  LIS_VECTOR x = nullptr;
  LIS_SOLVER solver = nullptr;

  lis_matrix_create(LIS_COMM_WORLD, &A);
  if (lis_input_matrix(A, const_cast<char *>(matrix_path)) != LIS_SUCCESS) {
    std::cerr << "LIS: Could not read matrix " << matrix_path << "\n";
  }

  lis_vector_create(LIS_COMM_WORLD, &b);
  if (lis_input_vector(b, const_cast<char *>(b_path)) != LIS_SUCCESS) {
    std::cerr << "LIS: Could not read vector " << b_path << "\n";
  }

  lis_vector_duplicate(A, &x);

  lis_solver_create(&solver);
  lis_solver_set_option(const_cast<char *>(options), solver);

  std::ostringstream tol_option;
  tol_option << "-tol " << std::scientific << tol;
  lis_solver_set_option(tol_option.str().data(), solver);

  lis_solve(A, b, x, solver);

  LIS_INT iterations = 0;
  LIS_REAL residual = 0.0;
  lis_solver_get_iter(solver, &iterations);
  lis_solver_get_residualnorm(solver, &residual);

  lis_output_vector(x, LIS_FMT_MM, const_cast<char *>(solution_path));

  lis_solver_destroy(solver);
  lis_vector_destroy(x);
  lis_vector_destroy(b);
  lis_matrix_destroy(A);
  lis_finalize();

  return {static_cast<int>(iterations), static_cast<double>(residual)};
}

} // namespace hpc
