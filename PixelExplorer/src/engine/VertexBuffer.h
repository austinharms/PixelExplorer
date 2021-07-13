#pragma once
#include "RefCounted.h"

class VertexBuffer : public virtual RefCounted {
 public:
  VertexBuffer();
  VertexBuffer(unsigned char size, unsigned int length, void* data);
  virtual ~VertexBuffer();
  void bind() const;
  void unbind() const;
  void updateVertices(unsigned char size, unsigned int length, void* data);
  void updateDirtyBuffer();
  unsigned int getGLID() const { return _renderId; }
  unsigned int getIndexCount() const { return _length; }
 private:
  void* _buffer;
  unsigned int _renderId;
  unsigned int _length;
  unsigned char _size;
  mutable bool _dirtyBuffer;
  void genGLBuffer();
  void updateGLBuffer() const;
};