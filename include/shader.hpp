#pragma once
#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <iterator>
#include <print>
#include <span>
#include <string_view>
#include <vector>
#include "linear_algrebra.hpp"
#include "utility.hpp"

namespace render {
  struct ShaderMetadata {
    std::filesystem::path _path;
    size_t _shader_type;
  };

  class Shader {
   private:
    using return_type = std::expected<uint32_t, bool>;
    static constexpr size_t kBufferSz{1024};

    enum class ErrorMsg : uint8_t {
      READERR,
      OPENERR,
      COMPILATIONERR,
      LINKERR,
      TOTAL_AMOUNT
    };

    static constexpr size_t kErrorMSGLength =
        utility::toSZ(ErrorMsg::TOTAL_AMOUNT);

    static constexpr std::array<std::string_view, kErrorMSGLength> kErrMsg{
        "Couldn't read non-existing file:", "Couldn't open file:",
        "Compilation error, shader:", "Linking error, shader:"};

    static return_type makeModule(const ShaderMetadata& metadata);
    static bool helper(const ShaderMetadata& metadata,
                       unsigned int& result_to_save);

    explicit Shader(size_t shader, std::span<std::string_view> _uniforms);

   public:
    ~Shader() { glDeleteProgram(_shader); }
    template <size_t N>
    static std::expected<Shader, bool>
    makeShader(std::span<ShaderMetadata, N> shaders_metadata,
               std::span<std::string_view> _uniforms)
    {
      std::array<unsigned int, N> modules{};
      auto* it = modules.begin();

      for (auto& metadata : shaders_metadata) {
        if (!helper(metadata, *it)) {
          return std::unexpected(false);
        }
        it = std::next(it);
      }

      unsigned int shader = glCreateProgram();
      for (auto& shader_module : modules) {
        glAttachShader(shader, shader_module);
      }

      glLinkProgram(shader);

      int result{};
      glGetProgramiv(shader, GL_LINK_STATUS, &result);

      if (result == 0) {
        std::array<char, kBufferSz> error_log{};
        glGetProgramInfoLog(shader, kBufferSz, nullptr, error_log.data());

        std::println("{} {}", kErrMsg[utility::toSZ(ErrorMsg::LINKERR)],
                     error_log);
      }

      for (auto& module : modules) {
        glDeleteShader(module);
      }

      return Shader{shader, _uniforms};
    }

    void use() const;

    void setBool(size_t idx, bool value) const;
    void setInt(size_t idx, int32_t value) const;
    void setBool(size_t idx, float value) const;
    void setMatrix4f(size_t idx, math::mat4F& value) const;

   private:
    std::vector<int> _locations;
    size_t _shader;
  };
}  // namespace render
