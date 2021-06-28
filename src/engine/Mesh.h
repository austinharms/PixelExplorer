#pragma once
#include <mutex>
#include <unordered_map>

#include "IndexBuffer.h"
#include "Material.h"
#include "RefCounted.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "glm/mat4x4.hpp"

class Mesh : public virtual RefCounted {
 public:
  Mesh();
  Mesh(VertexBufferAttrib* customAttribs[], unsigned short attribCount);
  virtual ~Mesh();
  void bind();
  void unbind() const;
  glm::mat4 getTransform() const { return _transform; }
  void setIndexCount(unsigned int count);
  unsigned int getIndexCount() const { return _currentIndexCount; }
  unsigned int getLatestIndexCount() const { return _latestIndexCount; }
  void setVertexCount(unsigned int count);
  unsigned int getVertexCount() const { return _currentVertexCount; }
  unsigned int getLatestVertexCount() const { return _latestVertexCount; }
  void setIndex(unsigned int index, unsigned int value);
  void setVertexPosition(unsigned int index, float x, float y, float z);
  void setVertexUV(unsigned int index, float u, float v);
  void setAttribVec2(unsigned short attribIndex, unsigned int index, float x,
                     float y);
  void setAttribVec3(unsigned short attribIndex, unsigned int index, float x,
                     float y, float z);
  void setRawAttribVec2(unsigned short attribOffset, unsigned int index,
                        float x, float y);
  void setRawAttribVec3(unsigned short attribOffset, unsigned int index,
                        float x, float y, float z);
  void updateBuffers();
  virtual void updateTransfrom(float dt){};

  VertexBuffer* getVertexBuffer() const { return _vertexBuffer; }

  unsigned int getID() const { return _id; }

  void setTransform(glm::mat4 t) { _transform = t; }

  void setMaterial(Material* m) {
    if (_material != nullptr) _material->drop();
    m->grab();
    _material = m;
  }

  bool hasMaterial() const { return _material != nullptr; }

  Material* getMaterial() const { return _material; }

  unsigned int getIndexType() const { return 0x1405; }

  void setRendererDropFlag(bool flag) { _rendererDropFlag = flag; }

  bool getRendererDropFlag() const { return _rendererDropFlag; }

  void lockMeshEditing() { _meshEditLock.lock(); }

  void unlockMeshEditing() { _meshEditLock.unlock(); }

  unsigned short getAttribOffset(unsigned short attribIndex) {
    return _vertexArray->getBufferAttribComponentOffset(0, attribIndex);
  }

  void unbindLock() { _meshBindLock.unlock(); }

 private:
  static unsigned int s_nextId;
  std::mutex _meshEditLock;
  std::mutex _meshBindLock;
  glm::mat4 _transform;
  std::unordered_map<unsigned short, unsigned short> _bufferOffsetMap;
  VertexBuffer* _vertexBuffer;
  IndexBuffer* _indexBuffer;
  VertexArray* _vertexArray;
  Material* _material;
  unsigned int _latestIndexCount;
  unsigned int _latestVertexCount;
  unsigned int _currentIndexCount;
  unsigned int _currentVertexCount;
  unsigned int* _latestIndices;
  float* _latestVertices;
  unsigned int* _currentIndices;
  float* _currentVertices;
  unsigned int _id;
  int _attribStride;
  bool _rendererDropFlag;
};