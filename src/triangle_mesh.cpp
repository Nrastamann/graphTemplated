#include "triangle_mesh.hpp"

#include <glad/gl.h>
#include <array>
#include "config.hpp"

#include <GLFW/glfw3.h>

TriangleMesh::TriangleMesh() : _element_count(6)
{
  std::array<float, 12> positions = {-1.f,  -1.f, 0.f,  1.0f, -1.0f, 0.f,
                                     -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  0.0f};

  std::array<int, 4> colorIdx     = {0, 1, 2, 3};
  std::array<int, 6> elementIdx   = {0, 1, 2, 2, 1, 3};

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float),
               positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementIdx.size() * sizeof(int),
               elementIdx.data(), GL_STATIC_DRAW);
}

void
TriangleMesh::draw()
{
  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, _element_count, GL_UNSIGNED_INT, (void*)0);
}

TriangleMesh::~TriangleMesh()
{
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
}
