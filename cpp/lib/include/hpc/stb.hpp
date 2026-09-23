#pragma once

#include <memory>
#include <stb_image.h>

namespace hpc {

struct StbiDeleter {
  void operator()(unsigned char *p) const { stbi_image_free(p); }
};

using StbiUPtr = std::unique_ptr<unsigned char, StbiDeleter>;

} // namespace hpc
