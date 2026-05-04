#pragma once
#include <cstdint>
#include <expected>
#include "shader.hpp"

namespace shaders {
  enum class CircleUniforms : uint8_t {
    Resolution,
    BorderThickness,
    Radius,
    LENGTH
  };
  enum class LineUniforms : uint8_t {
    Resolution,
    LENGTH
  };

  std::expected<render::Shader, bool> createCircleShader();
  std::expected<render::Shader, bool> createLineShader();
}  // namespace shaders
