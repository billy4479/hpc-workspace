#include <Eigen/Dense>
#include <Eigen/src/Core/ArithmeticSequence.h>
#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/Core/util/Constants.h>
#include <Eigen/src/Eigenvalues/GeneralizedEigenSolver.h>
#include <Eigen/src/SVD/BDCSVD.h>
#include <Eigen/src/SVD/SVDBase.h>
#include <filesystem>
#include <print>
#include <unsupported/Eigen/SparseExtra>

#include <hpc/stb.hpp>

using Image =
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using Eigen::MatrixXd;
using Eigen::VectorXd;

namespace {
Image load_image(const char *path) {
  constexpr int desired_channels = 1;

  int width = 0;
  int height = 0;
  int channels = 0;

  const auto *image =
      stbi_load(path, &width, &height, &channels, desired_channels);
  if (image == nullptr) {
    std::cerr << "Error: Could not load image " << path << "\n";
    return {};
  }

  Image img(height, width);
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      img(i, j) = static_cast<double>(image[(i * width) + j]);
    }
  }

  return img;
}

void save_image(const Image &img, const char *path) {
  Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      output_matrix(img.rows(), img.cols());

  output_matrix = img.unaryExpr([](double val) -> unsigned char {
    return static_cast<unsigned char>(std::clamp(val, 0.0, 255.0));
  });

  if (stbi_write_png(path, static_cast<int>(img.cols()),
                     static_cast<int>(img.rows()), 1, output_matrix.data(),
                     static_cast<int>(img.cols())) == 0) {
    std::cerr << "Error: Could not save grayscale image\n";
  }
}

} // namespace

void get_C_and_D(int k, const Eigen::BDCSVD<Image> &solver,
                 const char *save_path, const char *task) {
  auto C = solver.matrixU()(Eigen::all, Eigen::seq(0, k - 1));
  auto D = solver.matrixV()(Eigen::all, Eigen::seq(0, k - 1)) *
           solver.singularValues()(Eigen::seq(0, k - 1)).asDiagonal();

  auto multiplied = C * D.transpose();
  save_image(multiplied, save_path);

  std::println("- Task {}: k = {}, nnz(C) = {}, nnz(D) = {}, save_path = {}",
               task, k, C.nonZeros(), D.nonZeros(), save_path);
  std::println("\tC: {}x{}", C.rows(), C.cols());
  std::println("\tD: {}x{}", D.rows(), D.cols());
}

int main() {
  // Task 1
  Image input_image = load_image("./artifacts/deer.jpg");

  const auto ATA = input_image.transpose() * input_image;
  std::println("- Task 1: norm {}", ATA.norm());

  // Task 2
  const Eigen::BDCSVD<Image> svd_ata_solver(ATA);
  if (svd_ata_solver.info() != Eigen::ComputationInfo::Success) {
    std::cerr << "Computation failed\n";
  }

  auto eigenvalues = svd_ata_solver.singularValues().unaryExpr(
      [](const auto x) { return sqrt(x); });

  std::println("- Task 2: lambda_1 = {}, lambda_2 = {}", eigenvalues(0),
               eigenvalues(1));

  // Task 3
  constexpr const char *ata_path = "artifacts/ATA.mtx";
  if (!std::filesystem::exists(ata_path)) {
    Eigen::saveMarket(ATA, ata_path);
    std::println("- Task 3: saved to {}", ata_path);
  } else {
    std::println("- Task 3: using cached {}", ata_path);
  }

  constexpr const char *lis_command_task3 =
      "./build/cpp/apps/lis-eigen artifacts/ATA.mtx artifacts/eigenvectors.txt "
      "artifacts/history.txt -e pi "
      "-i cg -etol 1e-10";
  std::println("\t - command `{}`", lis_command_task3);

  // Task 4
  constexpr const char *lis_command_task4 =
      "./build/cpp/apps/lis-eigen artifacts/ATA.mtx artifacts/eigenvectors.txt "
      "artifacts/history.txt -e pi "
      "-i cg -etol 1e-10 -shift 5.6432e7";
  std::println("- Task 4: command `{}`", lis_command_task4);

  // Task 5
  const Eigen::BDCSVD<Image> svd_solver(input_image, Eigen::ComputeThinU |
                                                         Eigen::ComputeThinV);
  if (svd_solver.info() != Eigen::ComputationInfo::Success) {
    std::cerr << "Computation failed\n";
  }

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
  save_image(checkboard, checkboard_path);

  // Task 9
  auto noisy_checkboard = checkboard + Image::Random(l, l) * 60;
  constexpr const char *noisy_checkboard_path =
      "artifacts/noisy_checkboard.png";
  save_image(noisy_checkboard, noisy_checkboard_path);
  std::println("- Task 9: path = {}", noisy_checkboard_path);

  // Task 10
  const Eigen::BDCSVD<Image> svd_noisy_checkboard(
      noisy_checkboard, Eigen::ComputeThinU | Eigen::ComputeThinV);
  if (svd_noisy_checkboard.info() != Eigen::ComputationInfo::Success) {
    std::cerr << "Computation failed\n";
  }

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
