#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "material.hpp"
#include "triangle_mesh.hpp"

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include "linear_algrebra.hpp"
using return_type = std::expected<uint32_t, bool>;
namespace {
  constexpr size_t kLogSize{1024};
  constexpr size_t kHeight{480};
  constexpr size_t kWidth{640};

  constexpr float kRed{0.25F};
  constexpr float kGreen{0.5F};
  constexpr float kBlue{0.75F};
  constexpr float kAlpha{1.0F};

  constexpr float kRotateSpeed{25.F};
  constexpr float kFov{45.0F};
  constexpr float kNear{0.1F};
  constexpr float kFar{10.0F};

}  // namespace
static return_type
makeModule(const std::filesystem::path& filepath, uint32_t module)
{
  if (!exists(filepath)) {
    std::cout << "Couldn't read non-existing file: " << filepath << '\n';
    return std::unexpected(false);
  }
  std::ifstream file(filepath);
  std::string line;
  std::stringstream bufferedlines;

  if (!file.is_open()) {
    std::cout << "Couldn't open file:" << filepath.filename() << '\n';
    return std::unexpected(false);
  }

  while (std::getline(file, line)) {
    bufferedlines << line << '\n';
  }

  line                   = bufferedlines.str();
  const char* shader_src = line.c_str();

  uint32_t shader_module = glCreateShader(module);
  glShaderSource(shader_module, 1, &shader_src, nullptr);
  glCompileShader(shader_module);

  int result = 0;
  glGetShaderiv(shader_module, GL_COMPILE_STATUS, &result);
  if (result == 0) {
    std::array<char, kLogSize> error_log{};
    glGetShaderInfoLog(shader_module, kLogSize, nullptr, error_log.data());
    std::cout << "Shader module " << filepath.filename()
              << " compilation error:\n"
              << error_log.data() << '\n';
  }
  return shader_module;
}
static bool
helper(const std::filesystem::path& path, uint32_t module,
       unsigned int& result_to_save)
{
  std::expected<uint32_t, bool> shader = makeModule(path, module);

  if (!shader.has_value()) {
    return false;
  }

  result_to_save = shader.value();
  return true;
}

static return_type
makeShader(const std::filesystem::path& vertex_filepath,
           const std::filesystem::path& fragment_filepath)
{
  std::array<unsigned int, 2> modules{};

  if (!helper(vertex_filepath, GL_VERTEX_SHADER, modules[0])) {
    return std::unexpected(false);
  }

  if (!helper(fragment_filepath, GL_FRAGMENT_SHADER, modules[1])) {
    return std::unexpected(false);
  }

  unsigned int shader = glCreateProgram();
  for (auto& shader_module : modules) {
    glAttachShader(shader, shader_module);
  }
  glLinkProgram(shader);

  int result{};
  glGetProgramiv(shader, GL_LINK_STATUS, &result);
  if (result == 0) {
    std::array<char, kLogSize> error_log{};
    glGetProgramInfoLog(shader, kLogSize, nullptr, error_log.data());
    std::cout << "Shader Linking error:\n" << error_log.begin() << '\n';
  }

  for (auto& module : modules) {
    glDeleteShader(module);
  }

  return shader;
}

int
main()
{
  GLFWwindow* window = nullptr;
  if (glfwInit() == 0) {
    std::cout << "GLFW couldn't init window\n";
    return -1;
  }

  window = glfwCreateWindow(kWidth, kHeight, "GraphsLabs", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  if (gladLoadGL(glfwGetProcAddress) == 0) {
    std::cout << "Couldn't load opengl\n";
    glfwTerminate();
    return -1;
  }

  // Set up OpenGL
  glClearColor(kRed, kGreen, kBlue, kAlpha);
  // Set the rendering region to the actual screen size
  int w = 0;
  int h = 0;
  glfwGetFramebufferSize(window, &w, &h);
  //(left, top, width, height)
  glViewport(0, 0, w, h);

  TriangleMesh triangle;
  Material marika("imgs/GLIsReal.png");
  Material mask("imgs/mask.jpg");

  return_type shader =
      makeShader("src/shaders/vertex.vert", "src/shaders/fragment.frag");

  glUseProgram(shader.value());
  glUniform1i(glGetUniformLocation(shader.value(), "material"), 0);
  glUniform1i(glGetUniformLocation(shader.value(), "mask"), 1);

  constexpr math::vec3F kQuadPosition = {0.1F, -0.2F, 0.0F};
  constexpr math::vec3F kCameraPos    = {-5.0F, 0.0F, 3.0F};
  constexpr math::vec3F kCameraTarget = {0.0F, 0.0F, 0.0F};

  math::mat4F view       = math::createLookAt(kCameraPos, kCameraTarget);
  math::mat4F projection = math::createPerspectiveProjection<float>(
      kFov, static_cast<float>(kWidth) / static_cast<float>(kHeight), kNear,
      kFar);

  // fetch model matrix location
  auto model_location =
      static_cast<uint32_t>(glGetUniformLocation(shader.value(), "model"));
  auto view_location =
      static_cast<uint32_t>(glGetUniformLocation(shader.value(), "view"));
  auto proj_location =
      static_cast<uint32_t>(glGetUniformLocation(shader.value(), "projection"));

  glUniformMatrix4fv(static_cast<int32_t>(proj_location), 1, GL_FALSE,
                     projection.data().data_handle());
  glUniformMatrix4fv(static_cast<int32_t>(view_location), 1, GL_FALSE,
                     view.data().data_handle());

  // upload model matrix
  // configure alpha blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  while (glfwWindowShouldClose(window) == 0) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shader.value());

    math::mat4F model = math::createModelTransform(
        kQuadPosition, kRotateSpeed * static_cast<float>(glfwGetTime()));
    glUniformMatrix4fv(static_cast<int>(model_location), 1, GL_FALSE,
                       model.data().data_handle());

    marika.use(0);
    mask.use(1);
    triangle.draw();

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  if (shader.has_value()) {
    glDeleteProgram(shader.value());
  }
  glfwTerminate();
  return 0;
}
