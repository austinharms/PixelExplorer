#include "ChunkRenderMesh.h"

#include "GL/glew.h"
#include "Logger.h"
#include "glm/gtx/euler_angles.hpp"
namespace px::game::chunk {
ChunkRenderMesh::ChunkRenderMesh(rendering::Material* material) {
  _material = material;
  _material->grab();
  _vertexBuffer = nullptr;
  _indexBuffer = nullptr;
  _indexCount = 0;
  _active = true;
  _drop = false;
  _dirty = false;
  _error = false;
  _transform = glm::eulerAngleXYX(0, 0, 0);
  setPosition(glm::vec3(0));

  glGenVertexArrays(1, &_vertexArrayId);
  if (_vertexArrayId == 0) {
    _error = true;
    Logger::error("Failed to Create ChunkRenderMesh Vertex Array");
    return;
  }

  glBindVertexArray(_vertexArrayId);
  glGenBuffers(1, &_vertexBufferId);
  if (_vertexArrayId == 0) {
    _error = true;
    Logger::error("Failed to Create ChunkRenderMesh Vertex Buffer");
    return;
  }

  glGenBuffers(1, &_indexBufferId);
  if (_indexBufferId == 0) {
    _error = true;
    Logger::error("Failed to Create ChunkRenderMesh Index Buffer");
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
  _material->drop();
  glDeleteVertexArrays(1, &_vertexArrayId);
  glDeleteBuffers(1, &_vertexBufferId);
  glDeleteBuffers(1, &_indexBufferId);
  if (_vertexBuffer != nullptr) {
    _vertexBuffer->makeWriteable();
    _vertexBuffer->drop();
  }

  if (_indexBuffer != nullptr) {
    _indexBuffer->makeWriteable();
    _indexBuffer->drop();
  }
}

bool ChunkRenderMesh::shouldDrop() const { return _drop || _error; }

rendering::Material* ChunkRenderMesh::getMaterial() const { return _material; }

bool ChunkRenderMesh::preRender(float deltaTime, const glm::vec3& cameraPos,
                                const glm::vec3& cameraRotation) {
    return false;
  updateBuffers();
  return _active && !_error;
}

glm::mat4 ChunkRenderMesh::getTransform() const { return _transform; }

void ChunkRenderMesh::render() const {
  if (_error) return;
  glBindVertexArray(_vertexArrayId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
}

void ChunkRenderMesh::setPosition(glm::vec3 pos) {
  _transform[3] = glm::vec<4, float>(pos, 0);
}

void ChunkRenderMesh::setDropFlag() { _drop = true; }

void ChunkRenderMesh::updateBuffers(util::DataBuffer<float>* verts,
                                    util::DataBuffer<uint32_t>* indices) {
  if (_error) return;
  _bufferMutex.lock();
  _dirty = true;

  if (_vertexBuffer != nullptr) {
    _vertexBuffer->makeWriteable();
    _vertexBuffer->drop();
  }

  _vertexBuffer = verts;
  _indexBuffer->makeReadOnly();

  if (_indexBuffer != nullptr) {
    _indexBuffer->makeWriteable();
    _indexBuffer->drop();
  }

  _indexBuffer = indices;
  _indexBuffer->makeReadOnly();

  _bufferMutex.unlock();
}

bool ChunkRenderMesh::getError() const { return _error; }

void ChunkRenderMesh::setActive(bool active) { _active = active; }

void ChunkRenderMesh::updateBuffers() {
  if (!_dirty || _error) return;
  _bufferMutex.lock();
  _dirty = false;
  _indexCount = _indexBuffer->length;

  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, _vertexBuffer->getSize(), _vertexBuffer->buffer,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer->getSize(),
               _indexBuffer->buffer, GL_STATIC_DRAW);
  _vertexBuffer->makeWriteable();
  _vertexBuffer->drop();
  _vertexBuffer = nullptr;
  _indexBuffer->makeWriteable();
  _indexBuffer->drop();
  _indexBuffer = nullptr;
  _bufferMutex.unlock();
}
}  // namespace px::game::chunk