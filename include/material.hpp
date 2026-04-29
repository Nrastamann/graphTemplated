#pragma once

#include <filesystem>

class Material {
 public:
  Material(const Material&)            = default;
  Material(Material&&)                 = delete;
  Material& operator=(const Material&) = default;
  Material& operator=(Material&&)      = delete;
  explicit Material(const std::filesystem::path& path);
  ~Material();
  void use(int32_t unit) const;
  [[nodiscard]] bool
  getLoadStatus() const
  {
    return _error_code;
  }

 private:
  uint32_t _texture{};
  bool _error_code{false};
};
