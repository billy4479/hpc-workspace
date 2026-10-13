#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <cassert>
#include <print>
#include <unsupported/Eigen/SparseExtra>

#include <hpc/stb.hpp>

#include "convolution.hpp"
#include "image.hpp"
#include "lis_solve.hpp"
#include "mtx_vector.hpp"
#include "solvers.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using hpc::Image;

int main() {
  // Task 1
  Image input_image = hpc::load_image("./artifacts/deer.jpg");
  const auto rows = input_image.rows();
  const auto cols = input_image.cols();

  std::println("- Task 1: matrix size {}x{}", rows, cols);

  // Task 2
  constexpr auto noise_scale = 50.0 / 255.0;
  Image noise = Image::Random(rows, cols) * noise_scale;

  Image noisy_image = input_image + noise;
  constexpr auto noisy_image_path = "./artifacts/noisy.png";
  hpc::save_image(noisy_image, noisy_image_path);
  std::println("- Task 2: saved to {}", noisy_image_path);

  // Task 3: flatten row by row (row-major storage)
  VectorXd input_image_vec = input_image.reshaped<Eigen::RowMajor>();
  VectorXd noisy_image_vec = noisy_image.reshaped<Eigen::RowMajor>();

  assert(input_image_vec.size() == rows * cols);
  assert(input_image_vec.size() == noisy_image_vec.size());

  auto norm = input_image_vec.norm();
  std::println("- Task 3: norm of v {}", norm);

  // Task 4
  MatrixXd av1(3, 3);
  av1 << 1, 1, 1, 1, 4, 1, 1, 1, 1;
  av1 /= 12;

  auto matrix_av1 =
      hpc::build_convolution_matrix(av1, input_image_vec.size(), cols);
  std::println("- Task 4: nnz(av1) = {}", matrix_av1.nonZeros());

  // Task 5
  auto smoothed_noisy = matrix_av1 * noisy_image_vec;
  constexpr auto smoothed_noisy_path = "./artifacts/noisy_filtered.png";
  hpc::save_image_vec(smoothed_noisy, smoothed_noisy_path, cols);
  std::println("- Task 5: saved to {}", smoothed_noisy_path);

  // Task 6
  MatrixXd sh1(3, 3);
  sh1 << 0, -3, 0, -1, 9, -3, 0, -1, 0;

  auto matrix_sh1 =
      hpc::build_convolution_matrix(sh1, input_image_vec.size(), cols);
  std::println("- Task 6: nnz(sh1) = {}, symmetric: {}", matrix_sh1.nonZeros(),
               matrix_sh1.isApprox(matrix_sh1.transpose()));

  // Task 7
  auto sharpened_original = matrix_sh1 * input_image_vec;
  constexpr auto sharpened_original_path = "./artifacts/sharpened_original.png";
  hpc::save_image_vec(sharpened_original, sharpened_original_path, cols);
  std::println("- Task 7: saved to {}", sharpened_original_path);

  // Task 8
  constexpr auto sh1_mtx_path = "./artifacts/sh1.mtx";
  constexpr auto w_mtx_path = "./artifacts/w.mtx";
  Eigen::saveMarket(matrix_sh1, sh1_mtx_path);
  Eigen::saveMarketVector(noisy_image_vec, w_mtx_path);
  std::println("- Task 8: saved to {} and {}", sh1_mtx_path, w_mtx_path);

  // A2 is not symmetric (Task 6), so it can't be SPD: no CG, use BiCGSTAB.
  // Our LIS is serial, so this runs without mpirun.
  constexpr auto w_lis_path = "./artifacts/w_lis.mtx";
  constexpr auto x_lis_path = "./artifacts/x_lis.mtx";
  hpc::write_mtx_vector_lis(noisy_image_vec, w_lis_path);

  auto [lis_iterations, lis_residual] = hpc::solve_lis_file_output(
      sh1_mtx_path, w_lis_path, x_lis_path, "-i bicgstab -p ilu", 1e-12);
  std::println("- Task 8: LIS bicgstab + ilu, iterations: {}, residual: {}",
               lis_iterations, lis_residual);

  // Task 9
  VectorXd x = hpc::read_mtx_vector_lis(x_lis_path);
  constexpr auto x_image_path = "./artifacts/x.png";
  hpc::save_image_vec(x, x_image_path, cols);
  std::println("- Task 9: saved to {}", x_image_path);

  // Task 10
  MatrixXd ed2(3, 3);
  ed2 << -1, 0, 1, -2, 0, 2, -1, 0, 1;

  auto matrix_ed2 =
      hpc::build_convolution_matrix(ed2, input_image_vec.size(), cols);
  std::println("- Task 10: symmetric: {}",
               matrix_ed2.isApprox(matrix_ed2.transpose()));

  // Task 11
  auto edge_detected_original = matrix_ed2 * input_image_vec;
  constexpr auto edge_detected_original_path =
      "./artifacts/edge_detected_original.png";
  hpc::save_image_vec(edge_detected_original, edge_detected_original_path,
                      cols);
  std::println("- Task 11: saved to {}", edge_detected_original_path);

  // Task 12
  Eigen::SparseMatrix<double> identity(input_image_vec.size(),
                                       input_image_vec.size());
  identity.setIdentity();
  Eigen::SparseMatrix<double> matrix_task12 = 4.0 * identity + matrix_ed2;

  // CG needs A SPD, but 4I + A3 is never symmetric.
  std::println("- Task 12: 4I + A3 symmetric: {} -> not SPD, no CG",
               matrix_task12.isApprox(matrix_task12.transpose()));

  auto [y, eigen_iterations, eigen_residual] =
      hpc::solve_eigen_bicgstab(matrix_task12, noisy_image_vec, 1e-10);
  std::println("- Task 12: Eigen bicgstab, iterations: {}, residual: {}",
               eigen_iterations, eigen_residual);

  constexpr auto y_mtx_path = "./artifacts/y.mtx";
  Eigen::saveMarketVector(y, y_mtx_path);

  // Task 13
  VectorXd y_loaded;
  Eigen::loadMarketVector(y_loaded,
                          y_mtx_path); // Here we could even use y directly.
  constexpr auto y_image_path = "./artifacts/y.png";
  hpc::save_image_vec(y_loaded, y_image_path, cols);
  std::println("- Task 13: saved to {}", y_image_path);

  return 0;
}
