#pragma once

#include <algorithm>
#include <iostream>

#include <Eigen/Dense>

#include <hpc/stb.hpp>

namespace hpc {

// Grayscale image in [0, 1], stored row-major so that the linearized
// (flattened) form matches the usual row-by-row image layout.
using Image =
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

inline Image load_image(const char *path) {
  constexpr int desired_channels = 1;

  int width = 0;
  int height = 0;
  int channels = 0;

  StbiUPtr image(stbi_load(path, &width, &height, &channels, desired_channels));
  if (!image) {
    std::cerr << "Error: Could not load image " << path << "\n";
    return {};
  }

  Image img(height, width);
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      img(i, j) = static_cast<double>(image.get()[(i * width) + j]) / 255.0;
    }
  }

  return img;
}

inline void save_image(const Image &img, const char *path) {
  Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      output_matrix(img.rows(), img.cols());

  output_matrix = img.unaryExpr([](double val) -> unsigned char {
    return static_cast<unsigned char>(std::clamp(255.0 * val, 0.0, 255.0));
  });

  if (stbi_write_png(path, static_cast<int>(img.cols()),
                     static_cast<int>(img.rows()), 1, output_matrix.data(),
                     static_cast<int>(img.cols())) == 0) {
    std::cerr << "Error: Could not save grayscale image\n";
  }
}

inline void save_image_vec(const Eigen::VectorXd &img, const char *path,
                           Eigen::Index stride) {
  Eigen::Vector<unsigned char, Eigen::Dynamic> output_data =
      img.unaryExpr([](double val) -> unsigned char {
        return static_cast<unsigned char>(std::clamp(255.0 * val, 0.0, 255.0));
      });

  if (stbi_write_png(path, static_cast<int>(stride),
                     static_cast<int>(img.size() / stride), 1,
                     output_data.data(), static_cast<int>(stride)) == 0) {
    std::cerr << "Error: Could not save grayscale image\n";
  }
}

} // namespace hpc
