#pragma once
#include <array>
#include <cmath>
#include <cstddef>
#include <mdspan>
#include <numbers>

namespace math {
  static constexpr float kPiDegree{180.F};

  template <typename T>
  class Mat4 {
   public:
    static constexpr size_t kN{16};
    static constexpr size_t kDm{4};

    static_assert(kDm * kDm == kN, "WRONG DIMENSIONS");

    auto
    data()
    {
      return std::mdspan(_data.begin(), 4, 4);
    }

   private:
    std::array<T, kN> _data{};
  };
  template <typename T>
  using Vec3 = std::array<T, 3>;

  template <typename T>
  using Vec2  = std::array<T, 2>;
  using vec2F = Vec2<float>;

  template <typename T>
  Mat4<T>
  createMatrixTransform(Vec3<T> translation)
  {
    Mat4<T> matrix{};
    auto matrix_data = matrix.data();

    for (size_t i = 0; i < matrix.kDm; ++i) {
      matrix_data[i, i] = 1.0F;
    }

    matrix_data[3, 0] = translation[0];
    matrix_data[3, 1] = translation[1];
    matrix_data[3, 2] = translation[2];

    return matrix;
  }
  template <typename T>
  Mat4<T>
  createZRotation(float angle)
  {
    angle = std::numbers::pi_v<float> * angle / kPiDegree;
    Mat4<T> matrix{};
    auto matrix_data  = matrix.data();

    float c           = cosf(angle);
    float s           = sinf(angle);

    matrix_data[0, 0] = c;
    matrix_data[0, 1] = s;

    matrix_data[1, 0] = -s;
    matrix_data[1, 1] = c;

    // c -s 0 0
    // s c 0 0
    // 0 0 0 0
    // 0 0 0 0

    matrix_data[2, 2] = 1.0F;  // c -s 0 0
    matrix_data[3, 3] = 1.0F;  // s c 0 0
                               // 0 0 1 0
                               // 0 0 0 1

    return matrix;
  }
  template <typename T>
  Mat4<T>
  createModelTransform(Vec3<T> pos, float angle)
  {
    angle = std::numbers::pi_v<float> * angle / kPiDegree;
    Mat4<T> matrix{};
    auto matrix_data  = matrix.data();

    float c           = cosf(angle);
    float s           = sinf(angle);

    matrix_data[0, 0] = c;
    matrix_data[0, 1] = s;

    matrix_data[1, 0] = -s;
    matrix_data[1, 1] = c;

    // c -s 0 0
    // s c 0 0
    // 0 0 0 0
    // 0 0 0 0

    matrix_data[3, 0] = pos[0];
    matrix_data[3, 1] = pos[1];
    matrix_data[3, 2] = pos[2];

    // c -s 0 a
    // s c 0 b
    // 0 0 0 d
    // 0 0 0 0

    matrix_data[2, 2] = 1.0F;  // c -s 0 a
    matrix_data[3, 3] = 1.0F;  // s c 0 b
                               // 0 0 1 d
                               // 0 0 0 1

    return matrix;
  }

  template <typename T>
  float
  dot(Vec3<T> u, Vec3<T> v)
  {
    return (u[0] * v[0]) + (u[1] * v[1]) + (v[2] * u[2]);
  }

  template <typename T>
  Vec3<T>
  normalize(Vec3<T> v)
  {
    Vec3<T> u{};

    float magnitude = sqrtf(dot(v, v));

    u[0]            = v[0] / magnitude;
    u[1]            = v[1] / magnitude;
    u[2]            = v[2] / magnitude;

    return u;
  }

  template <typename T>
  Vec3<T>
  cross(Vec3<T> u, Vec3<T> v)
  {
    Vec3<T> w;

    w[0] = (u[1] * v[2]) - (u[2] * v[1]);
    w[1] = -(u[0] * v[2]) - (u[2] * v[0]);
    w[2] = (u[0] * v[1]) - (u[1] * v[0]);

    return w;
  }

  template <typename T>
  Mat4<T>
  createLookAt(Vec3<T> from, Vec3<T> to)
  {
    Vec3<T> global_up = {0.0F, 0.0F, 1.0F};
    Vec3<T> f         = {
        to[0] - from[0],
        to[1] - from[1],
        to[2] - from[2],
    };
    f         = normalize(f);

    Vec3<T> r = normalize(cross(f, global_up));
    Vec3<T> u = normalize(cross(r, f));

    Mat4<T> matrix{};

    auto data  = matrix.data();

    data[0, 0] = r[0];
    data[0, 1] = u[0];
    data[0, 2] = -f[0];

    data[1, 0] = r[1];
    data[1, 1] = u[1];
    data[1, 2] = -f[1];

    data[2, 0] = r[2];
    data[2, 1] = u[2];
    data[2, 2] = -f[2];

    data[3, 0] = -dot(r, from);
    data[3, 1] = -dot(u, from);
    data[3, 2] = dot(f, from);
    data[3, 3] = 1.0F;

    return matrix;
  }

  template <typename T>
  Mat4<T>
  createPerspectiveProjection(float fovy, float aspect, float near, float far)
  {
    constexpr float kDoublePI{2.F * kPiDegree};
    fovy    = fovy * std::numbers::pi_v<float> / kDoublePI;

    float t = tanf(fovy);
    float n = -near;
    float f = -far;

    Mat4<T> matrix{};

    auto data  = matrix.data();

    // 0 4 8 12
    // 1 5 9 13
    // 2 6 10 14
    // 3 7 11 15
    data[0, 0] = 1.0F / (aspect * t);
    data[1, 1] = 1.0F / t;
    data[2, 2] = -(n + f) / (n - f);
    data[2, 3] = -1.0F;
    data[3, 2] = 2 * n * f / (n - f);

    return matrix;
  }

  using mat4F = Mat4<float>;
  using vec3F = Vec3<float>;
}  // namespace math
