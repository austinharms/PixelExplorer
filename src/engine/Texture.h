#pragma once
#include <string>
#include "RefCounted.h"

class Texture : public virtual RefCounted {
 public:
  Texture(unsigned int width, unsigned int height);
  Texture(const char* path);
  virtual ~Texture();
  void bind(unsigned int slot = 0) const;
  void unbind() const;
  int getWidth() const { return _width; };
  int getHeight() const { return _height; };
  unsigned int getGLID() const { return _renderId; }

 private:
  int _width;
  int _height;
  int _bpp;
  void* _buffer;
  unsigned int _renderId;
  void loadTex(const char* path);
  void createGLTexture();
};