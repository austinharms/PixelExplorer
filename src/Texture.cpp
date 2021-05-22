#include "Texture.h"

#include <GL/glew.h>
#include "stb_image/stb_image.h"

Texture::Texture(unsigned int width, unsigned int height)
    : _renderId(0), _width(width), _height(height), _bpp(0), _buffer(nullptr) {
  createGLTexture();
}

Texture::Texture(std::string path)
    : _renderId(0), _width(0), _height(0), _bpp(0), _buffer(nullptr) {
  loadTex(path);
  createGLTexture();
}

Texture::~Texture() { 
  if (_buffer != nullptr)
    stbi_image_free(_buffer); 
  glDeleteTextures(1, &_renderId);
}

void Texture::bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, _renderId);
}

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::loadTex(std::string path) { 
  stbi_set_flip_vertically_on_load(1); 
  _buffer =
      stbi_load("./res/textures/textures.png", &_width, &_height, &_bpp, 4);
}

void Texture::createGLTexture() {
  glGenTextures(1, &_renderId);
  glBindTexture(GL_TEXTURE_2D, _renderId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, _buffer);
  unbind();
}

//load texture
/*
int tWidth = 0;
int tHeight = 0;
int BPP = 0;
stbi_set_flip_vertically_on_load(1);
void* imgBuffer =
    stbi_load("./res/textures/textures.png", &tWidth, &tHeight, &BPP, 4);
unsigned int texId;
glGenTextures(1, &texId);
glBindTexture(GL_TEXTURE_2D, texId);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tWidth, tHeight, 0, GL_RGBA,
             GL_UNSIGNED_BYTE, imgBuffer);
glBindTexture(GL_TEXTURE_2D, 0);
stbi_image_free(imgBuffer);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texId);
*/