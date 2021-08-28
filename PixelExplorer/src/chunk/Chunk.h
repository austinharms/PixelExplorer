#ifndef CHUNK_H
#define CHUNK_H

#include "GL/glew.h"
#include "RefCounted.h"
#include "block/Block.h"
#include "glm/mat4x4.hpp"
#include "rendering/Renderable.h"

class Chunk : public virtual RefCounted, public virtual Renderable {
 public:
  static const int CHUNK_SIZE = 25;
  static const int LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
  static const int BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;

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

 private:
  enum class Face : char { TOP, BOTTOM, LEFT, RIGHT, FRONT, BACK, FACECOUNT };

  void updateBuffers();  // MUST be called on main thread

  float* _vertexBuffer;
  unsigned short* _buffers[(int)Face::FACECOUNT];
  Block _blocks[BLOCK_COUNT];
  bool _buffersDirty;
  unsigned int _indexBuffers[(int)Face::FACECOUNT];
  unsigned int _vertexArrayId;
  unsigned int _vertexBufferId;
  int _vertexCount;
  unsigned short _indexCount[(int)Face::FACECOUNT];
  unsigned short _currentIndexCount[(int)Face::FACECOUNT];

  glm::vec3 _position;
  glm::mat4 _transform;
};

#endif  // !CHUNK_H
