#include "TexturedMaterial.h"

#include "GL/glew.h"
namespace px::rendering {
TexturedMaterial::TexturedMaterial(Shader* shader, void* texture, int32_t width,
                                   int32_t height) {
  _shader = shader;
  _shader->grab();
  _width = width;
  _height = height;
  glGenTextures(1, &_textureId);
  glBindTexture(GL_TEXTURE_2D, _textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, texture);
  unbindTexture();
}

TexturedMaterial::~TexturedMaterial() {
  unbindTexture();
  glDeleteTextures(1, &_textureId);
}

void TexturedMaterial::bind() {
  bindTexture();
  if (_shader != nullptr) _shader->setUniform1i("u_Texture", 0);
}

void TexturedMaterial::updateTexture(void* texture, int32_t width,
                                     int32_t height) {
  _width = width;
  _height = height;
  glBindTexture(GL_TEXTURE_2D, _textureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, texture);
  unbindTexture();
}

void TexturedMaterial::bindTexture() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textureId);
}

void TexturedMaterial::unbindTexture() { glBindTexture(GL_TEXTURE_2D, 0); }
}  // namespace px::rendering
