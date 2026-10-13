#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace hpc {

// Builds the sparse matrix A such that A * flatten(img) is the 2D
// convolution of img with `filter`, using zero padding. The image is
// flattened row by row and has `stride` columns.
inline Eigen::SparseMatrix<double>
build_convolution_matrix(const Eigen::MatrixXd &filter, Eigen::Index size,
                         Eigen::Index stride) {
  Eigen::SparseMatrix<double> A(size, size);
  A.reserve(Eigen::VectorXi::Constant(
      size, static_cast<int>((filter.array() != 0.0).count())));

  const Eigen::Index row_mid = filter.rows() / 2;
  const Eigen::Index col_mid = filter.cols() / 2;

  const Eigen::Index cols = stride;
  const Eigen::Index rows = size / stride;

  for (Eigen::Index k = 0; k < size; k++) {
    for (Eigen::Index i = -row_mid; i <= row_mid; i++) {
      for (Eigen::Index j = -col_mid; j <= col_mid; j++) {
        const Eigen::Index nr = (k / cols) + i;
        const Eigen::Index nc = (k % cols) + j;

        // Check rows and columns separately: a global bounds check on the
        // linear index would let the filter wrap around image borders.
        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
          continue;
        }

        const double value = filter(i + row_mid, j + col_mid);
        if (value != 0.0) {
          A.insert(k, nc + (stride * nr)) = value;
        }
      }
    }
  }

  return A;
}

} // namespace hpc
