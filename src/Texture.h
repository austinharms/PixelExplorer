#pragma once
#include <string>

class Texture {
 public:
  Texture(unsigned int width, unsigned int height);
  Texture(std::string path);
  ~Texture();
  void Bind(unsigned int slot = 0) const;
  void Unbind() const;
  int getWidth() const { return _width; };
  int getHeight() const { return _height; };
 private:
  int _width;
  int _height;
  int _bpp;
  void* _buffer;
  unsigned int _renderId;
  void loadTex(std::string path);
  void createGLTexture();
};