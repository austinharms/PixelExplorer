#pragma once
#include "RefCounted.h"

class IndexBuffer : public virtual RefCounted {
 public:
  IndexBuffer();
  IndexBuffer(unsigned char size, unsigned int length, void* data);
  virtual ~IndexBuffer();
  void bind() const;
  void unbind() const;
  void updateIndices(unsigned char size, unsigned int length, void* data);
  unsigned int getGLID() const { return _renderId; }

 private:
  mutable bool _dirtyBuffer;
  unsigned char _size;
  void* _buffer;
  unsigned int _length;
  unsigned int _renderId;
  void genGLBuffer();
  void updateGLBuffer() const;
};