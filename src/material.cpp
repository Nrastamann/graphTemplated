#include "material.hpp"
#include <filesystem>
#include <iostream>
#include <memory>
#include "glad/gl.h"

#include "stb_image.h"

Material::Material(const std::filesystem::path& path)
{
  int width{};
  int height{};
  int channels{};

  auto img = std::unique_ptr<unsigned char, void (*)(unsigned char*)>(
      stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha),
      [](unsigned char* ptr) { stbi_image_free(ptr); });

  if (img == nullptr) {
    _error_code = true;
    std::cout << "COULDN'T LOAD PICTURE: " << path.filename() << "\n";

    return;
  }

  glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_2D, _texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, img.get());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST);  // shrink img sampled
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  GL_LINEAR);  // shrink img sampled
}

Material::~Material() { glDeleteTextures(1, &_texture); }

void
Material::use(int32_t unit) const
{
  glActiveTexture(GL_TEXTURE0 + static_cast<int>(unit));
  glBindTexture(GL_TEXTURE_2D, _texture);
}
