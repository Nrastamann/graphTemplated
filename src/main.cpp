#include <cstdint>
#include <expected>
#include <filesystem>
#include <iostream>

#include "material.hpp"
#include "shader.hpp"
#include "triangle_mesh.hpp"

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include "linear_algrebra.hpp"
using return_type = std::expected<uint32_t, bool>;
namespace {
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
  auto shader_wrapped = createCircleShader();

  if (!shader_wrapped.has_value()) {
    glfwTerminate();
    return -1;
  }
  Shader& circle_shader = shader_wrapped.value();

  circle_shader.use();
  circle_shader.setInt(toSZ(CircleUniforms::MATERIAL), 0);
  circle_shader.setInt(toSZ(CircleUniforms::MASK), 1);

  constexpr math::vec3F kQuadPosition = {0.1F, -0.2F, 0.0F};
  constexpr math::vec3F kCameraPos    = {-5.0F, 0.0F, 3.0F};
  constexpr math::vec3F kCameraTarget = {0.0F, 0.0F, 0.0F};

  math::mat4F view       = math::createLookAt(kCameraPos, kCameraTarget);
  math::mat4F projection = math::createPerspectiveProjection<float>(
      kFov, static_cast<float>(kWidth) / static_cast<float>(kHeight), kNear,
      kFar);

  // fetch model matrix location
  circle_shader.setMatrix4f(toSZ(CircleUniforms::PROJECTION), projection);
  circle_shader.setMatrix4f(toSZ(CircleUniforms::VIEW), view);

  // upload model matrix
  // configure alpha blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  while (glfwWindowShouldClose(window) == 0) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);
    circle_shader.use();

    math::mat4F model = math::createModelTransform(
        kQuadPosition, kRotateSpeed * static_cast<float>(glfwGetTime()));
    circle_shader.setMatrix4f(toSZ(CircleUniforms::MODEL), model);

    marika.use(0);
    mask.use(1);
    triangle.draw();

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
