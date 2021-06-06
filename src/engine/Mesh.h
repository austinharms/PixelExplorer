#pragma once
#include <glm/mat4x4.hpp>

#include "IndexBuffer.h"
#include "Material.h"
#include "RefCounted.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

class Mesh : public virtual RefCounted {
 public:
  Mesh();
  Mesh(VertexBufferAttrib* customAttribs[], unsigned short attribCount);
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
  void setAttribVec2(unsigned short attribIndex, unsigned int index, float x,
                     float y);
  void setAttribVec3(unsigned short attribIndex, unsigned int index, float x,
                     float y, float z);
  void updateBuffers();
  void updateTransfrom(float dt);
  VertexBuffer* getVertexBuffer() const { return _vertexBuffer; }
  unsigned int getID() const { return _id; }
  void setTransform(glm::mat4 t) { _transform = t; }
  void setMaterial(Material* m) {
    if (_material != nullptr) _material->drop();
    m->grab();
    _material = m;
  }
  Material* getMaterial() const { return _material; }

 private:
  static unsigned int s_nextId;
  glm::mat4 _transform;
  VertexBuffer* _vertexBuffer;
  IndexBuffer* _indexBuffer;
  VertexArray* _vertexArray;
  Material* _material;
  unsigned int _indexCount;
  unsigned int _vertexCount;
  unsigned short* _indices;
  float* _vertices;
  unsigned int _id;
  int _attribStride;
};