#include "Chunk.h"
#include <chunk/block/Block.h>

Chunk::Chunk() {
  setPosition(glm::vec3(0));
  _visible = true;
  _vertexBuffer = nullptr;
  _vertexCount = 0;
  for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
    _buffers[i] = nullptr;
    _indexCount[i] = 0;
    _currentIndexCount[i] = 0;
  }

  _buffersDirty = false;
  glGenVertexArrays(1, &_vertexArrayId);
  glBindVertexArray(_vertexArrayId);
  glGenBuffers(1, &_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5,
                        (const void*)(sizeof(float) * 3));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenBuffers((int)FaceDirection::FACECOUNT, _indexBuffers);
}

Chunk::~Chunk() {}

bool Chunk::onPreRender(float deltaTime, float* cameraPos,
                        float* cameraRotation) {
  if (_buffersDirty) updateBuffers();
  _visibleFaces = FaceDirectionFlag::ALL;
  return _visible;
}

void Chunk::onRender() {
  glBindVertexArray(_vertexArrayId);
  for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
    if (!(_visibleFaces & FaceDirectionFlag::DirectionToFlag(i)) ||
        !_currentIndexCount[i])
      continue;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffers[i]);
    glDrawElements(GL_TRIANGLES, _currentIndexCount[i], GL_UNSIGNED_SHORT,
                   nullptr);
  }
}

void Chunk::UpdateMesh() { 
  std::lock_guard<std::mutex> lock(_meshBuffersLock);

  //Clear Old Buffer Data
  if (_vertexBuffer != nullptr) {
    free(_vertexBuffer);
    _vertexBuffer = nullptr;
  }

  _vertexCount = 0;

  for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
    if (_buffers[i] != nullptr) {
      free(_buffers[i]);
      _buffers[i] = nullptr;
    }

    _indexCount[i] = 0;
  }


  uint8_t x = 0;
  uint8_t y = 0;
  uint8_t z = 0;
  Block curBlock(this);

  for (uint32_t i = 0; i < Chunk::BLOCK_COUNT; ++i) {
    //Front Faces
    

    if (++x == Chunk::CHUNK_SIZE) {
      x = 0;
      if (++z == Chunk::CHUNK_SIZE) {
        z = 0;
        ++y;
      }
    }
  }

}

void Chunk::updateBuffers() {
  if (_meshBuffersLock.try_lock()) {
    if (_vertexBuffer != nullptr) {
      glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
      glBufferData(GL_ARRAY_BUFFER, _vertexCount * 5 * sizeof(float),
                   _vertexBuffer, GL_STATIC_DRAW);
      free(_vertexBuffer);
      _vertexBuffer = nullptr;
    }

    for (char i = 0; i < (char)FaceDirection::FACECOUNT; ++i) {
      if (_buffers[i] != nullptr) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffers[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     _indexCount[i] * sizeof(unsigned short), _buffers[i],
                     GL_STATIC_DRAW);
        _currentIndexCount[i] = _indexCount[i];
        _indexCount[i] = 0;
        free(_buffers[i]);
        _buffers[i] = nullptr;
      }
    }

    _buffersDirty = false;
    _meshBuffersLock.unlock();
  }
}
