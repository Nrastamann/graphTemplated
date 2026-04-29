#pragma once
#include <cstddef>

namespace utility {
  template <typename U>
  static constexpr size_t
  toSZ(U val)
  {
    return static_cast<size_t>(val);
  }

};  // namespace utility
