#pragma once
#include "RefCounted.h"

class VertexBuffer : public virtual RefCounted {
 public:
  VertexBuffer();
  virtual ~VertexBuffer();
  void bind() const;
  void unbind() const;
  void updateVertices(unsigned int size, void* data);
  void updateGLBuffer() const;
  unsigned int getGLID() const { return _renderId; }

 private:
  mutable bool _dirtyBuffer;
  unsigned int _renderId;
  void* _buffer;
  unsigned int _size;
  void genGLBuffer();
};