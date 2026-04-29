#include "app.hpp"
#include "linear_algrebra.hpp"
#include "shader_declaration.hpp"
namespace app {
  bool
  Renderer::loadShaders()
  {
    auto circle_shader = shaders::createCircleShader();
    if (!circle_shader.has_value()) {
      return false;
    }

    _shaders.push_back(circle_shader.value());
    return true;
  }

  size_t
  Renderer::makeCircleMesh(math::vec2F position, float radius)
  {
  }

}  // namespace app
