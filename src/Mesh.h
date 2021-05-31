#pragma once
#include "RefCounted.h"
#include <glm/mat4x4.hpp>
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

class Mesh : public virtual RefCounted{
 public:
  Mesh();
  virtual ~Mesh();
  void bind() const;
  void unbind() const;
  glm::mat4 getTransform() const { return _transform; }
  void setIndexCount(unsigned int count);
  unsigned int getIndexCount() const { return _indexCount; }
  void setVertexCount(unsigned int count);
  unsigned int getVertexCount() const { return _vertexCount; }
  void setIndex(unsigned int index, unsigned short value);
  void setVertexPosition(unsigned int index, float x, float y, float z);
  void setVertexUV(unsigned int index, float u, float v);
  void updateBuffers();
  VertexBuffer* getVertexBuffer() const { return _vertexBuffer; }
 private:
  glm::mat4 _transform;
  VertexBuffer* _vertexBuffer;
  IndexBuffer* _indexBuffer;
  VertexArray* _vertexArray;
  unsigned int _indexCount;
  unsigned int _vertexCount;
  unsigned short* _indices;
  float* _vertices;
};