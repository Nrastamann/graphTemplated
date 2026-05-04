#include "shader.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
namespace render {
  Shader::Shader(size_t shader, std::span<std::string_view> _uniforms) :
      _shader(shader), _delete(true)
  {
    _locations.resize(_uniforms.size());
    auto it = _locations.begin();

    for (auto& name : _uniforms) {
      use();
      *it = glGetUniformLocation(_shader, name.data());

      std::advance(it, 1);
    }
  }

  Shader::return_type
  Shader::makeModule(const ShaderMetadata& metadata)
  {
    if (!exists(metadata._path)) {
      std::println(std::cout, "{0} {1}",
                   kErrMsg[utility::toSZ(ErrorMsg::READERR)],
                   metadata._path.filename().c_str());

      return std::unexpected(false);
    }

    std::ifstream file(metadata._path);
    std::string line;
    std::stringstream bufferedlines;

    if (!file.is_open()) {
      std::println(std::cout, "{} {}",
                   kErrMsg[utility::toSZ(ErrorMsg::OPENERR)],
                   metadata._path.filename().c_str());

      return std::unexpected(false);
    }

    while (std::getline(file, line)) {
      bufferedlines << line << '\n';
    }

    line                   = bufferedlines.str();

    const char* shader_src = line.c_str();

    uint32_t shader_module = glCreateShader(metadata._shader_type);
    glShaderSource(shader_module, 1, &shader_src, nullptr);
    glCompileShader(shader_module);

    int result = 0;

    glGetShaderiv(shader_module, GL_COMPILE_STATUS, &result);

    if (result == 0) {
      std::array<char, kBufferSz> error_log{};
      glGetShaderInfoLog(shader_module, kBufferSz, nullptr, error_log.data());
      std::println("{} {} {}", kErrMsg[utility::toSZ(ErrorMsg::COMPILATIONERR)],
                   metadata._path.filename().c_str(),
                   std::string_view(error_log));  // mayber error with error log
    }
    return shader_module;
  }

  bool
  Shader::helper(const ShaderMetadata& metadata, unsigned int& result_to_save)
  {
    std::expected<uint32_t, bool> shader = makeModule(metadata);

    if (!shader.has_value()) {
      return false;
    }

    result_to_save = shader.value();
    return true;
  }

  void
  Shader::use() const
  {
    glUseProgram(_shader);
  }

  void
  Shader::setBool(size_t idx, bool value) const
  {
    glUniform1i(_locations[idx], static_cast<int32_t>(value));
  }

  void
  Shader::setInt(size_t idx, int32_t value) const
  {
    glUniform1i(_locations[idx], value);
  }
  void
  Shader::setFloat(size_t idx, float value) const
  {
    glUniform1f(_locations[idx], value);
  }
  void
  Shader::setMatrix4f(size_t idx, math::mat4F& value) const
  {
    glUniformMatrix4fv(_locations[idx], 1, GL_FALSE,
                       value.data().data_handle());
  }

  void
  Shader::setMatrix2f(size_t idx, math::vec2F& value) const
  {
    glUniform2f(_locations[idx], value[0], value[1]);
  }
  void
  Shader::setFloatArr(size_t idx, size_t N, std::array<float, 49>& value) const
  {
    glUniform1fv(_locations[idx], static_cast<int32_t>(49), value.data());
  }
}  // namespace render
