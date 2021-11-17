#ifndef CHUNK_H
#define CHUNK_H

#include <unordered_map>
#include <cstdint>

#include "GL/glew.h"
#include "RefCounted.h"
#include "block/ChunkBlock.h"
#include "block/BlockData.h"
#include "FaceDirection.h"
#include "glm/mat4x4.hpp"
#include "rendering/Renderable.h"

class Chunk : public virtual RefCounted, public virtual Renderable {
 public:
  static const int32_t CHUNK_SIZE = 25;
  static const int32_t LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
  static const int32_t BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;

  Chunk();           // MUST be called on main thread
  virtual ~Chunk();  // MUST be called on main thread
  bool onPreRender(float deltaTime, float* cameraPos, float* cameraRotation);
  void onRender();
  void setPosition(glm::vec3 position) {
    _position = position;
    _transform =
        glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                  1.0f, 0.0f, _position.x, _position.y, _position.z, 1.0f);
  }

  glm::vec3 getPosition() const { return _position; }

  BlockData* getBlockData(uint32_t blockIndex) {
    return &_extendedBlockData.at(blockIndex);
  }

 private:
  void updateBuffers();  // MUST be called on main thread

  float* _vertexBuffer;
  unsigned short* _buffers[(int32_t)FaceDirection::FACECOUNT];
  std::unordered_map<uint32_t, BlockData> _extendedBlockData;
  ChunkBlock _blocks[BLOCK_COUNT];
  bool _buffersDirty;
  uint8_t _visibleFaces;
  uint32_t _indexBuffers[(int32_t)FaceDirection::FACECOUNT];
  uint32_t _vertexArrayId;
  uint32_t _vertexBufferId;
  int32_t _vertexCount;
  uint16_t _indexCount[(int32_t)FaceDirection::FACECOUNT];
  uint16_t _currentIndexCount[(int32_t)FaceDirection::FACECOUNT];

  glm::vec3 _position;
  glm::mat4 _transform;
};

#endif  // !CHUNK_H
