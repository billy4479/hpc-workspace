#include <Eigen/Dense>
#include <Eigen/src/Core/ArithmeticSequence.h>
#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/Eigenvalues/GeneralizedEigenSolver.h>
#include <Eigen/src/SVD/BDCSVD.h>
#include <Eigen/src/SVD/SVDBase.h>
#include <print>
#include <unsupported/Eigen/SparseExtra>

#include <hpc/stb.hpp>

#include "image.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using hpc::Image;

void get_C_and_D(int k, const Eigen::BDCSVD<Image> &solver,
                 const char *save_path, const char *task) {
  auto C = solver.matrixU()(Eigen::all, Eigen::seq(0, k - 1));
  auto D = solver.matrixV()(Eigen::all, Eigen::seq(0, k - 1)) *
           solver.singularValues()(Eigen::seq(0, k - 1)).asDiagonal();

  auto multiplied = C * D.transpose();
  hpc::save_image(multiplied, save_path);

  std::println("- Task {}: k = {}, nnz(C) = {}, nnz(D) = {}, save_path = {}",
               task, k, C.nonZeros(), D.nonZeros(), save_path);
  std::println("\tC: {}x{}", C.rows(), C.cols());
  std::println("\tD: {}x{}", D.rows(), D.cols());
}

int main() {
  // Task 1
  Image input_image = hpc::load_image("./artifacts/deer.jpg");

  const auto ATA = input_image.transpose() * input_image;
  std::println("- Task 1: norm {}", ATA.norm());

  // Task 2
  // Eigen::EigenSolver<Image> solver(ATA);
  // auto eigenvalues = solver.eigenvalues();
  // std::println("- Task 2: lambda_1 = {} + {}, lambda_2 = {} + {}",
  //              eigenvalues(0).real(), eigenvalues(0).imag(),
  //              eigenvalues(1).real(), eigenvalues(1).imag());

  // Task 3
  // Eigen::saveMarket(ATA, "artifacts/ATA.mtx");
  // ../build/cpp/apps/lis-eigen ATA.mtx eigenvectors.txt history.txt -e pi -i \
  //  cg -etol 1e-10

  // Task 4
  // ../build/cpp/apps/lis-eigen ATA.mtx eigenvectors.txt history.txt -e ii -i \
  //  cg -etol 1e-10 -shift 1e-10

  // Task 5
  const Eigen::BDCSVD<Image> svd_solver(input_image, Eigen::ComputeThinU |
                                                         Eigen::ComputeThinV);
  std::cout << svd_solver.info() << "\n";

  const auto &Sigma = svd_solver.singularValues();
  std::println("- Task 5: norm(Sigma) = {}", Sigma.norm());

  // Task 6 & 7
  get_C_and_D(30, svd_solver, "artifacts/deer_CD_30.png", "6 & 7");
  get_C_and_D(90, svd_solver, "artifacts/deer_CD_90.png", "6 & 7");

  // Task 8
  constexpr auto l = 400;
  constexpr auto n_cells = 10;
  constexpr auto pixels_per_cell = l / n_cells;
  MatrixXd checkboard = MatrixXd::Zero(l, l);
  for (int i = 0; i < l; i++) {
    for (int j = 0; j < l; j++) {
      auto checkboard_x = i / pixels_per_cell;
      auto checkboard_y = j / pixels_per_cell;
      if ((checkboard_x + checkboard_y) % 2 == 1) {
        checkboard(i, j) = 255;
      }
    }
  }
  std::println("- Task 8: norm = {}", checkboard.norm());
  constexpr const char *checkboard_path = "artifacts/checkboard.png";
  hpc::save_image(checkboard, checkboard_path);

  // Task 9
  auto noisy_checkboard = checkboard + Image::Random(l, l) * 60;
  constexpr const char *noisy_checkboard_path =
      "artifacts/noisy_checkboard.png";
  hpc::save_image(noisy_checkboard, noisy_checkboard_path);
  std::println("- Task 9: path = {}", noisy_checkboard_path);

  // Task 10
  const Eigen::BDCSVD<Image> svd_noisy_checkboard(
      noisy_checkboard, Eigen::ComputeThinU | Eigen::ComputeThinV);
  std::cout << svd_noisy_checkboard.info() << "\n";

  std::println("- Task 10: sigma_1 = {}, sigma_2 = {}",
               svd_noisy_checkboard.singularValues()(0),
               svd_noisy_checkboard.singularValues()(1));

  // Task 11
  get_C_and_D(6, svd_noisy_checkboard, "artifacts/noisy_checkboard_CD_06.png",
              "11 & 12");
  get_C_and_D(12, svd_noisy_checkboard, "artifacts/noisy_checkboard_CD_12.png",
              "11 & 12");

  return 0;
}
