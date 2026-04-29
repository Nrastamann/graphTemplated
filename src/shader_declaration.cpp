#include "shader_declaration.hpp"
namespace shaders {
  std::expected<render::Shader, bool>
  createCircleShader()
  {
    std::array<render::ShaderMetadata, 2> metadata({
        {         ._path = "src/shaders/circle.vert",._shader_type = GL_VERTEX_SHADER          },
        {._path        = "src/shaders/fragment.frag",
         ._shader_type = GL_FRAGMENT_SHADER}
    });

    std::array<std::string_view, utility::toSZ(CircleUniforms::LENGTH)>
        uniforms = {"material", "mask", "model", "view", "projection"};

    return render::Shader::makeShader(std::span(metadata), std::span(uniforms));
  }
}  // namespace shaders
