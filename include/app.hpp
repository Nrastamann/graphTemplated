#pragma once
#include <filesystem>
#include <vector>
#include "linear_algrebra.hpp"
#include "shader.hpp"

using System = std::variant<uint32_t>;
namespace app {
  class Renderer {
   public:
    bool loadShaders();
    size_t makeCircleMesh(math::vec2F position, float radius);
    size_t makeCubeMesh(math::vec3F size);
    size_t makeTexture(std::filesystem::path& path);
    void setUpOpengl();

   private:
    std::vector<size_t> _vaos;
    std::vector<size_t> _vbos;
    std::vector<size_t> _textures;
    std::vector<render::Shader> _shaders;
  };

  class App {
   public:
    App(const App&)            = delete;
    App(App&&)                 = delete;
    App& operator=(const App&) = delete;
    App& operator=(App&&)      = delete;

    App();
    size_t makeEntity();
    void makeSystems();
    ~App();

    void run();

   private:
    void setUpGlfw();

    std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> _window;
    std::vector<System> _systems;
    size_t _entity_count{};
  };
}  // namespace app
