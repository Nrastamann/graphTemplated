#pragma once
#include <cstdint>
#include <expected>
#include "shader.hpp"

namespace shaders {
  enum class CircleUniforms : uint8_t {
    MATERIAL,
    MASK,
    MODEL,
    VIEW,
    PROJECTION,
    LENGTH
  };

  std::expected<render::Shader, bool> createCircleShader();
}  // namespace shaders
