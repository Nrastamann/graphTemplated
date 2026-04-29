#pragma once

#include <array>
#include <cstdint>

class TriangleMesh {
 public:
  TriangleMesh();
  void draw();
  ~TriangleMesh();

 private:
  unsigned int _vbo{};
  unsigned int _ebo{};
  unsigned int _vao{};
  unsigned int _element_count{};
};
