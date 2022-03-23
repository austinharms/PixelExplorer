#include "ChunkRenderMesh.h"

#include "GL/glew.h"
#include "Logger.h"
#include "glm/gtx/euler_angles.hpp"

inline void ChunkRenderMesh::SetMaterial(Material* mat) {
  if (s_material != nullptr) s_material->drop();
  s_material = mat;
  if (s_material != nullptr) s_material->grab();
}

inline void ChunkRenderMesh::DropMaterial() {
  if (s_material != nullptr) s_material->drop();
  s_material = nullptr;
}

ChunkRenderMesh::ChunkRenderMesh() {
  _vertexBuffer = nullptr;
  _indexBuffer = nullptr;
  _indexCount = 0;
  _active = true;
  _drop = false;
  _dirty = false;
  _error = false;
  _transform = glm::eulerAngleXYX(0, 0, 0);
  SetPosition(glm::vec3(0));

  glGenVertexArrays(1, &_vertexArrayId);
  if (_vertexArrayId == 0) {
    _error = true;
    Logger::Error("Failed to Create ChunkRenderMesh Vertex Array");
    return;
  }

  glBindVertexArray(_vertexArrayId);
  glGenBuffers(1, &_vertexBufferId);
  if (_vertexArrayId == 0) {
    _error = true;
    Logger::Error("Failed to Create ChunkRenderMesh Vertex Buffer");
    return;
  }

  glGenBuffers(0, &_indexBufferId);
  if (_indexBufferId == 0) {
    _error = true;
    Logger::Error("Failed to Create ChunkRenderMesh Index Buffer");
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5,
                        (const void*)(sizeof(float) * 3));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

ChunkRenderMesh::~ChunkRenderMesh() {
  glDeleteVertexArrays(1, &_vertexArrayId);
  glDeleteBuffers(1, &_vertexBufferId);
  glDeleteBuffers(1, &_indexBufferId);
  if (_vertexBuffer != nullptr) {
    _vertexBuffer->ReadOnly = false;
    _vertexBuffer->drop();
  }

  if (_indexBuffer != nullptr) {
    _indexBuffer->ReadOnly = false;
    _indexBuffer->drop();
  }
}

inline bool ChunkRenderMesh::ShouldDrop() const { return _drop || _error; }

inline Material* ChunkRenderMesh::GetMaterial() const { return s_material; }

inline bool ChunkRenderMesh::PreRender(float deltaTime,
                                       const glm::vec3& cameraPos,
                                       const glm::vec3& cameraRotation) {
  UpdateBuffers();
  return _active && !_error;
}

inline glm::mat4 ChunkRenderMesh::GetTransform() const { return _transform; }

void ChunkRenderMesh::Render() const {
  if (_error) return;
  glBindVertexArray(_vertexArrayId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
}

inline void ChunkRenderMesh::SetPosition(glm::vec3 pos) {
  _transform[3] = glm::vec<4, float>(pos, 0);
}

inline void ChunkRenderMesh::SetDropFlag() { _drop = true; }

inline void ChunkRenderMesh::UpdateBuffers(DataBuffer<float>* verts,
                                           DataBuffer<uint32_t>* indices) {
  if (_error) return;
  _bufferMutex.lock();
  _dirty = true;

  if (_vertexBuffer != nullptr) {
    _vertexBuffer->ReadOnly = false;
    _vertexBuffer->drop();
  }

  _vertexBuffer = verts;
  _indexBuffer->ReadOnly = true;

  if (_indexBuffer != nullptr) {
    _indexBuffer->ReadOnly = false;
    _indexBuffer->drop();
  }

  _indexBuffer = indices;
  _indexBuffer->ReadOnly = true;

  _bufferMutex.unlock();
}

inline bool ChunkRenderMesh::GetError() const { return _error; }

inline void ChunkRenderMesh::SetActive(bool active) { _active = active; }

void ChunkRenderMesh::UpdateBuffers() {
  if (!_dirty || _error) return;
  _bufferMutex.lock();
  _dirty = false;
  _indexCount = _indexBuffer->Length;

  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, _vertexBuffer->GetSize(), _vertexBuffer->Buffer,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer->GetSize(),
               _indexBuffer->Buffer, GL_STATIC_DRAW);
  _vertexBuffer->ReadOnly = false;
  _vertexBuffer->drop();
  _vertexBuffer = nullptr;
  _indexBuffer->ReadOnly = false;
  _indexBuffer->drop();
  _indexBuffer = nullptr;
  _bufferMutex.unlock();
}