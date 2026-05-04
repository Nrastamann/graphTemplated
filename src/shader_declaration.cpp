#include "shader_declaration.hpp"
namespace shaders {
  std::expected<render::Shader, bool>
  createCircleShader()
  {
    std::array<render::ShaderMetadata, 2> metadata({
        {       ._path = "src/shaders/circle.vert",._shader_type = GL_VERTEX_SHADER          },
        {._path        = "src/shaders/circle.frag",
         ._shader_type = GL_FRAGMENT_SHADER}
    });

    std::array<std::string_view, utility::toSZ(CircleUniforms::LENGTH)>
        uniforms = {"u_resolution", "borderThickness", "radius"};

    return render::Shader::makeShader(std::span(metadata), std::span(uniforms));
  }
  std::expected<render::Shader, bool>
  createLineShader()
  {
    std::array<render::ShaderMetadata, 2> metadata({
        {       ._path = "src/shaders/line.vert",._shader_type = GL_VERTEX_SHADER          },
        {._path        = "src/shaders/line.frag",
         ._shader_type = GL_FRAGMENT_SHADER}
    });

    std::array<std::string_view, utility::toSZ(LineUniforms::LENGTH)> uniforms =
        {"u_resolution"};

    return render::Shader::makeShader(std::span(metadata), std::span(uniforms));
  }
}  // namespace shaders
