#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <expected>
#include <limits>
#include <print>
#include "linear_algrebra.hpp"
#include "shader.hpp"
#include "shader_declaration.hpp"
#include "utility.hpp"
namespace visual {
  constexpr size_t kHeight{1080};
  constexpr size_t kWidth{1920};

  constexpr float kHeightF{1080.F};
  constexpr float kWidthF{1920.F};

  struct CircleMesh {
   private:
    static constexpr size_t kFrameVertexes{6};
    static constexpr size_t kNodes{4};
    static constexpr size_t kVBOCount{5};
    static constexpr float kDefaultColour{0.5F};
    static constexpr float kDefaultCenter{512.F};
    static constexpr float kDefaultColourBorder{0.3F};

    static constexpr std::array<uint32_t, kFrameVertexes> kElements{0, 1, 3,
                                                                    3, 2, 0};
    enum class DataPools : uint8_t {
      CircleCenter,
      Colour,
      BorderColour,
      Vertexes,
      Lines
    };

    static constexpr std::array<size_t, kVBOCount> kStrides{8, 12, 12, 8, 4};
    static constexpr std::array<size_t, kVBOCount> kSizes{
        2 * kNodes, 3 * kNodes, 3 * kNodes, 2 * kNodes, kNodes};

    static constexpr std::array<float,
                                kSizes[utility::toSZ(DataPools::Vertexes)]>
        kTriangles{0.0F, 0.0F,     kWidthF, 0.0F,
                   0.0F, kHeightF, kWidthF, kHeightF};

    std::array<uint32_t, kVBOCount> _vbo{};
    size_t _idx;
    uint32_t _vao{};
    uint32_t _ebo{};

    bool _activated{false};

    std::array<float, kSizes[utility::toSZ(DataPools::CircleCenter)]> _center{
        kDefaultCenter, kDefaultCenter, kDefaultCenter, kDefaultCenter,
        kDefaultCenter, kDefaultCenter, kDefaultCenter, kDefaultCenter};

    std::array<float, kSizes[utility::toSZ(DataPools::Colour)]> _colours{
        kDefaultColour, kDefaultColour, kDefaultColour, kDefaultColour,
        kDefaultColour, kDefaultColour, kDefaultColour, kDefaultColour,
        kDefaultColour, kDefaultColour, kDefaultColour, kDefaultColour,
    };

    std::array<float, kSizes[utility::toSZ(DataPools::BorderColour)]>
        _border_colours{
            kDefaultColourBorder, kDefaultColourBorder, kDefaultColourBorder,
            kDefaultColourBorder, kDefaultColourBorder, kDefaultColourBorder,
            kDefaultColourBorder, kDefaultColourBorder, kDefaultColourBorder,
            kDefaultColourBorder, kDefaultColourBorder, kDefaultColourBorder,
        };

    std::array<float, kSizes[utility::toSZ(DataPools::Lines)]> _line_flag{
        -1.0, -1.0, -1.0, -1.0};

   public:
    void
    print(size_t idx)
    {
      std::println(
          "Current {} Node:\n\tCenter position - {}\n\tColours - {}\n\tBorder "
          "colours - {}\n\tLine flag - {}\n\tVBO - {}\n\tVAO - {}\n\tActivated "
          "- {}",
          idx, _center, _colours, _border_colours, _line_flag, _vbo, _vao,
          _activated);
    }
    void
    markAsLine(const math::vec3F& pointComponents, float value)
    {
      size_t idx = utility::toSZ(DataPools::Lines);

      setColourBorder(pointComponents);

      std::ranges::for_each(
          _line_flag, [value](auto& value_to_set) { value_to_set = value; });

      glNamedBufferSubData(_vbo.at(idx), 0,
                           static_cast<int64_t>(kSizes.at(idx) * sizeof(float)),
                           _line_flag.data());
    }
    CircleMesh()
    {
      static size_t circle_number = 0;
      _idx                        = circle_number++;
    }
    CircleMesh(const CircleMesh&) = delete;
    CircleMesh(CircleMesh&& mesh) noexcept :
        _vbo(mesh._vbo),
        _idx(mesh._idx),
        _vao(mesh._vao),
        _ebo(mesh._ebo),
        _activated(true),
        _center(mesh._center),
        _colours(mesh._colours),
        _border_colours(mesh._border_colours),
        _line_flag(mesh._line_flag)
    {
      mesh._activated = false;
    }

    CircleMesh& operator=(const CircleMesh&) = delete;
    CircleMesh& operator=(CircleMesh&&)      = default;
    void
    setIdx(size_t idx)
    {
      _idx = idx;
    }
    [[nodiscard]] size_t
    getIdx() const
    {
      return _idx;
    }

    [[nodiscard]] float
    distance(math::vec2F pos) const
    {
      float x_diff = _center[0] - pos[0];
      float y_diff = _center[1] - pos[1];

      return (x_diff * x_diff) + (y_diff * y_diff);
    }

    void
    setCenterPosition(math::vec2F pos)
    {
      size_t idx = 0;
      std::ranges::generate(_center,
                            [&idx, pos]() { return pos.at((idx++) & 1); });
      idx = utility::toSZ(DataPools::CircleCenter);
      glNamedBufferSubData(_vbo.at(idx), 0,
                           static_cast<int64_t>(kSizes.at(idx) * sizeof(float)),
                           _center.data());
    }
    math::vec2F
    getCenterPos()
    {
      return {_center[0], _center[1]};
    }

    void
    setColour(const math::vec3F& colour)
    {
      size_t idx = 0;
      std::ranges::generate(
          _colours, [&idx, &colour]() { return colour.at((idx++) % 3); });
      idx = utility::toSZ(DataPools::Colour);
      glNamedBufferSubData(_vbo.at(idx), 0,
                           static_cast<int64_t>(kSizes.at(idx) * sizeof(float)),
                           _colours.data());
    }

    void
    setColourBorder(const math::vec3F& colour)
    {
      size_t idx = 0;
      std::ranges::generate(_border_colours, [&idx, &colour]() {
        return colour.at((idx++) % 3);
      });
      idx = utility::toSZ(DataPools::BorderColour);
      glNamedBufferSubData(_vbo.at(idx), 0,
                           static_cast<int64_t>(kSizes.at(idx) * sizeof(float)),
                           _border_colours.data());
    }

    void
    activate()
    {
      _activated = true;
      glGenVertexArrays(1, &_vao);
      glBindVertexArray(_vao);

      std::array<const float*, kVBOCount> its{
          _center.data(), _colours.data(), _border_colours.data(),
          kTriangles.data(), _line_flag.data()};

      size_t idx = 0;

      for (auto& vbo : _vbo) {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<int64_t>(sizeof(float) * kSizes.at(idx)),
                     its.at(idx), GL_STATIC_DRAW);

        glVertexAttribPointer(
            idx, static_cast<int32_t>(kSizes.at(idx) / sizeof(float)), GL_FLOAT,
            GL_FALSE, static_cast<int32_t>(kStrides.at(idx)), nullptr);
        glEnableVertexAttribArray(idx++);
      }

      glGenBuffers(1, &_ebo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, kElements.size() * sizeof(int),
                   kElements.data(), GL_STATIC_DRAW);
    }

    void
    draw() const
    {
      glBindVertexArray(_vao);

      glDrawElements(GL_TRIANGLES, kFrameVertexes, GL_UNSIGNED_INT, nullptr);
    }

    ~CircleMesh()
    {
      if (_activated) {
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(static_cast<int32_t>(_vbo.size()), _vbo.data());
        glDeleteBuffers(1, &_ebo);
      }
    }
  };
  enum class ShaderTypes : uint8_t {
    Circle,
    Line
  };

  class GraphRenderer {
    explicit GraphRenderer(render::Shader&& shaderCircle, render::Shader&&,
                           size_t N)
    {
      _shader.emplace_back(std::move(shaderCircle));
      //_shader.emplace_back(std::move(shaderLine));

      _shader[0].use();

      _drawData.resize(N);
      for (auto& draw_data : _drawData) {
        draw_data.activate();
      }
    }

   public:
    void
    resize(size_t N)
    {
      size_t prev_size = _drawData.size();

      _drawData.resize(N);

      for (auto it = _drawData.begin() + static_cast<int64_t>(prev_size);
           it != _drawData.end(); std::advance(it, 1)) {
        it->activate();
      }
    }
    auto
    findCircle(size_t idx)
    {
      return std::ranges::find(_drawData, idx,
                               [](auto& _data) { return _data.getIdx(); });
    }

    static std::expected<GraphRenderer, bool>
    createGraph(size_t N)
    {
      auto shader_wrapped = shaders::createCircleShader();
      if (!shader_wrapped.has_value()) {
        glfwTerminate();
        return std::unexpected(false);
      }

      auto line_shader_wrapped = shaders::createLineShader();
      if (!line_shader_wrapped.has_value()) {
        glfwTerminate();
        return std::unexpected(false);
      }

      return GraphRenderer(std::move(shader_wrapped.value()),
                           std::move(line_shader_wrapped.value()), N);
    }

    void
    addNode()
    {
      _shader[0].use();

      _drawData.emplace_back().activate();
    }

    math::vec2F
    getPoint(size_t idx)
    {
      return findCircle(idx)->getCenterPos();
    }
    size_t
    getNearest(math::vec2F pos)
    {
      size_t current_max{0};
      size_t idx = 0;
      float distance{std::numeric_limits<float>::max()};
      std::ranges::for_each(_drawData, [&current_max, pos, &distance,
                                        &idx](const CircleMesh& draw_data) {
        auto distance_cur = draw_data.distance(pos);
        if (distance_cur < distance) {
          current_max = idx;
          distance    = distance_cur;
        }
        idx++;
      });

      return current_max;
    }
    void
    use(ShaderTypes shader_type)
    {
      _shader[utility::toSZ(shader_type)].use();
    }

    void
    setPosition(size_t idx, math::vec2F pos)
    {
      use(ShaderTypes::Circle);

      auto sz = findCircle(idx);

      sz->setCenterPosition(pos);
    }
    void
    setColour(size_t idx, math::vec3F colour)
    {
      use(ShaderTypes::Circle);
      _drawData.at(idx).setColour(colour);
    }
    void
    setColourBorder(size_t idx, math::vec3F colour)
    {
      use(ShaderTypes::Circle);
      _drawData.at(idx).setColourBorder(colour);
    }
    void
    draw()
    {
      std::ranges::for_each(_drawData, [this](auto& data) {
        this->use(ShaderTypes::Circle);
        data.draw();
      });
    }

    void
    setRadius(float radius)
    {
      use(ShaderTypes::Circle);
      _shader[utility::toSZ(ShaderTypes::Circle)].setFloat(
          utility::toSZ(shaders::CircleUniforms::Radius), radius);
    }
    void
    setResolution(math::vec2F resolution)
    {
      use(ShaderTypes::Circle);
      _shader[utility::toSZ(ShaderTypes::Circle)].setMatrix2f(
          utility::toSZ(shaders::CircleUniforms::Resolution), resolution);
    }

    void
    setBorderThickness(float thickness)
    {
      use(ShaderTypes::Circle);
      _shader[utility::toSZ(ShaderTypes::Circle)].setFloat(
          utility::toSZ(shaders::CircleUniforms::BorderThickness), thickness);
    }
    void
    setLine(size_t idx, const math::vec4F& pointComponents)
    {
      _drawData.at(idx).markAsLine(
          {
              pointComponents[0],
              pointComponents[1],
              pointComponents[2],
          },
          pointComponents[3]);
    }
    void
    printMeshState(size_t idx)
    {
      _drawData[idx].print(idx);
    }

   private:
    std::vector<render::Shader> _shader;
    std::vector<CircleMesh> _drawData;
  };
}  // namespace visual
