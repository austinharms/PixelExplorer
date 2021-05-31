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
  unsigned int getIndexType() const { return 0x1403; }
  unsigned int getIndexCount() const { return _length; }
 private:
  mutable bool _dirtyBuffer;
  unsigned int _renderId;
  void* _buffer;
  unsigned char _size;
  unsigned int _length;
  void genGLBuffer();
  void updateGLBuffer() const;
};