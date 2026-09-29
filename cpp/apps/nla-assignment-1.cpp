#include <Eigen/Eigen>
#include <cassert>
#include <iostream>
#include <print>
#include <unsupported/Eigen/SparseExtra>

#include <hpc/stb.hpp>
#include <unsupported/Eigen/src/SparseExtra/MarketIO.h>

using Eigen::MatrixXd;
using Eigen::VectorXd;

MatrixXd load_image(const char *path) {
  int desired_channels = 1;
  int x = 0;
  int y = 0;
  int channels = 0;

  auto *image = stbi_load(path, &x, &y, &channels, desired_channels);

  MatrixXd img(x, y);
  for (int i = 0; i < y; i++) {
    for (int j = 0; j < x; j++) {
      int index = (i * x + j) * channels;
      img(j, i) = static_cast<double>(image[index]) / 255.0;
    }
  }

  stbi_image_free(image);

  return img;
}

void save_image_vec(const VectorXd &img, const char *path,
                    Eigen::Index stride) {
  Eigen::Vector<unsigned char, Eigen::Dynamic> output_data =
      img.unaryExpr([](double val) -> unsigned char {
        return static_cast<unsigned char>(val * 255.0);
      });

  if (stbi_write_png(path, img.size() / stride, stride, 1, output_data.data(),
                     stride) == 0) {
    std::cerr << "Error: Could not save grayscale image\n";
  }
}

void save_image(const MatrixXd &img, const char *path) {
  Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> output_matrix(
      img.rows(), img.cols());

  output_matrix = img.unaryExpr([](double val) -> unsigned char {
    return static_cast<unsigned char>(val * 255.0);
  });

  if (stbi_write_png(path, img.rows(), img.cols(), 1, output_matrix.data(),
                     img.rows()) == 0) {
    std::cerr << "Error: Could not save grayscale image\n";
  }
}

auto build_convolution_matrix(const MatrixXd &filter, Eigen::Index size,
                              Eigen::Index stride) {
  auto A = Eigen::SparseMatrix<double>(size, size);
  A.reserve(Eigen::VectorXi::Constant(size, filter.size()));

  auto row_mid = filter.rows() / 2;
  auto col_mid = filter.cols() / 2;

  for (auto k = 0; k < size; k++) {
    for (auto i = -row_mid; i <= row_mid; i++) {
      for (auto j = -col_mid; j <= col_mid; j++) {
        if (k + i + (j * stride) < 0 || k + i + (j * stride) >= size) {
          continue;
        }

        A.insert(k, k + i + (j * stride)) = filter(i + row_mid, j + col_mid);
      }
    }
  }

  return A;
}

int main() {
  // Task 1
  auto input_image = load_image("./artifacts/deer.jpg");
  const auto rows = input_image.rows();
  const auto cols = input_image.cols();

  std::println("- Task 1: matrix size {}x{}", rows, cols);

  // Task 2
  constexpr auto noise_scale = 50.0 / 255.0;
  MatrixXd noise = (MatrixXd::Random(rows, cols) * noise_scale).array();

  auto noisy_image = input_image + noise;
  constexpr auto noisy_image_path = "./artifacts/noisy.png";
  save_image(noisy_image, noisy_image_path);
  std::println("- Task 2: saved to {}", noisy_image_path);

  // Task 3
  VectorXd input_image_vec = input_image.reshaped();
  VectorXd noisy_image_vec = noisy_image.reshaped();

  assert(input_image_vec.size() == rows * cols);
  assert(input_image_vec.size() == noisy_image_vec.size());

  auto norm = input_image_vec.norm();
  std::println("- Task 3: norm of v {}", norm);

  // Task 4
  MatrixXd av1(3, 3);
  av1 << 1, 1, 1, 1, 4, 1, 1, 1, 1;
  av1 /= 12;

  auto matrix_av1 = build_convolution_matrix(av1, input_image_vec.size(), cols);
  std::println("- Task 4: nnz(av1) = {}", matrix_av1.nonZeros());

  // Task 5
  auto smoothed_noisy = matrix_av1 * noisy_image_vec;
  constexpr auto smoothed_noisy_path = "./artifacts/noisy_filtered.png";
  save_image_vec(smoothed_noisy, smoothed_noisy_path, cols);
  std::println("- Task 5: saved to {}", smoothed_noisy_path);

  // Task 6
  MatrixXd sh1(3, 3);
  sh1 << 0, -3, 0, -1, 9, -3, 0, -1, 0;

  auto matrix_sh1 = build_convolution_matrix(sh1, input_image_vec.size(), cols);
  std::println("- Task 6: nnz(av1) = {}, symmetric: {}", matrix_sh1.nonZeros(),
               matrix_sh1.isApprox(matrix_sh1.transpose()));

  // Task 7
  auto sharpened_original = matrix_sh1 * input_image_vec;
  constexpr auto sharpened_original_path = "./artifacts/sharpened_original.png";
  save_image_vec(sharpened_original, sharpened_original_path, cols);
  std::println("- Task 7: saved to {}", sharpened_original_path);

  // Task 8
  constexpr auto sh1_mtx_path = "./artifacts/sh1.mtx";
  constexpr auto w_mtx_path = "./artifacts/w.mtx";
  Eigen::saveMarket(matrix_sh1, sh1_mtx_path);
  Eigen::saveMarketVector(noisy_image_vec, w_mtx_path);
  std::println("- Task 8: saved to {} and {}", sh1_mtx_path, w_mtx_path);

  // Task 9 - TODO
  std::println("- Task 9: TODO");

  // Task 10
  MatrixXd ed1(3, 3);
  ed1 << 0, -1, 0, -1, 4, -1, 0, -1, 0;
  auto matrix_ed1 = build_convolution_matrix(ed1, input_image_vec.size(), cols);
  std::println("- Task 10: symmetric: {}",
               matrix_ed1.isApprox(matrix_ed1.transpose()));

  // Task 11
  auto edge_detected_original = matrix_ed1 * input_image_vec;
  constexpr auto edge_detected_original_path =
      "./artifacts/edge_detected_original.png";
  save_image_vec(edge_detected_original, edge_detected_original_path, cols);
  std::println("- Task 11: saved to {}", edge_detected_original_path);

  // Task 12 - TODO
  std::println("- Task 12: TODO");

  // Task 13 - TODO
  std::println("- Task 13: TODO");

  return 0;
}
