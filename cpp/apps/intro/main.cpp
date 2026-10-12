#include <Eigen/Eigen>
#include <format>
#include <iostream>
#include <stb_image_write.h>

#include <hpc/stb.hpp>

using Eigen::MatrixXd;
using Eigen::VectorXd;

MatrixXd example_mat(int n) {
  MatrixXd A = MatrixXd::Zero(n, n);

  for (int i = 0; i < n; i++) {
    A(i, i) = 2;
    if (i + 1 < n) {
      A(i, i + 1) = -1;
      A(i + 1, i) = -1;
    }
  }
  return A;
}

int main() {
  int desired_channels = 1;
  int x = 0;
  int y = 0;
  int channels = 0;
  hpc::StbiUPtr image(stbi_load("/home/billy/Downloads/deer.jpg", &x, &y,
                                &channels, desired_channels));
  std::cout << std::format("Loaded {} channels, desired channels {}\n",
                           channels, desired_channels);

  MatrixXd img(x, y);

  for (int i = 0; i < y; i++) {
    for (int j = 0; j < x; j++) {
      int index = (i * x + j) * channels;
      img(x - i - 1, y - j - 1) =
          static_cast<double>(image.get()[index]) / 255.0;
    }
  }

  Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      output_matrix(x, y);
  output_matrix = img.unaryExpr([](double val) -> unsigned char {
    return static_cast<unsigned char>(val * 255.0);
  });

  const std::string output_image_path = "/tmp/output.png";
  if (stbi_write_png(output_image_path.c_str(), x, y, channels,
                     output_matrix.data(), x) == 0) {
    std::cerr << "Error: Could not save grayscale image\n";

    return 1;
  }
}
