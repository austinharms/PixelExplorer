#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <mutex>
#include <unordered_map>

#include "FaceDirection.h"
#include "GL/glew.h"
#include "RefCounted.h"
#include "block/BlockData.h"
#include "block/ChunkBlock.h"
#include "glm/mat4x4.hpp"
#include "rendering/Renderable.h"
#include "glm/gtx/euler_angles.hpp"

class Chunk : public virtual RefCounted, public virtual Renderable {
 public:
     enum class Status : uint8_t {
         CREATED = 0,
         UNLOADED = 1,
         LOADING = 2,
         UNLOADING = 3,
         LOADED = 4,
         UPDATING = 5
     };

  static const int32_t CHUNK_SIZE = 25;
  static const int32_t LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
  static const int32_t BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;

  Chunk();           // MUST be called on main thread
  virtual ~Chunk();  // MUST be called on main thread
  bool onPreRender(float deltaTime, float* cameraPos, float* cameraRotation);
  void onRender();
  void updateMesh();

  void setPosition(glm::vec3 position) {
    _position = position;
    _transform = glm::eulerAngleYXZ(0,0,0);
    _transform[3][0] = _position.x * Chunk::CHUNK_SIZE;
    _transform[3][1] = _position.y * Chunk::CHUNK_SIZE;
    _transform[3][2] = _position.z * Chunk::CHUNK_SIZE;
  }

  glm::vec3 getPosition() const { return _position; }

  glm::mat4 getTransform() { return _transform; }

  BlockData* getBlockData(uint32_t blockIndex) {
    return &_extendedBlockData.at(blockIndex);
  }

  const Status getStatus() const { return _status; }

  void setStatus(Status status) { _status = status; }

  static void freeEmptyBuffer() {
    if (s_emptyBuffer != nullptr) free(s_emptyBuffer);
  }

 private:
  static void* s_emptyBuffer;

  void updateBuffers();  // MUST be called on main thread

  Chunk* _adjacentChunks[(int32_t)FaceDirection::FACECOUNT];
  float* _vertexBuffer;
  std::mutex _meshBuffersLock;
  uint32_t* _buffers[(int32_t)FaceDirection::FACECOUNT];
  std::unordered_map<uint32_t, BlockData> _extendedBlockData;
  ChunkBlock _blocks[BLOCK_COUNT];
  bool _buffersDirty;
  uint8_t _visibleFaces;
  Status _status;
  uint32_t _indexBuffers[(int32_t)FaceDirection::FACECOUNT];
  uint32_t _vertexArrayId;
  uint32_t _vertexBufferId;
  uint32_t _vertexCount;
  uint32_t _indexCount[(int32_t)FaceDirection::FACECOUNT];
  uint32_t _currentIndexCount[(int32_t)FaceDirection::FACECOUNT];

  glm::vec3 _position;
  glm::mat4 _transform;
};

#endif  // !CHUNK_H
