#ifndef BLOCKFACE_H
#define BLOCKFACE_H
#include <cstdint>

#include "../FaceDirection.h"
#include "RefCounted.h"

struct BlockFace {
 public:
  BlockFace() {
    indices = nullptr;
    vertices = nullptr;
    uvs = nullptr;
    direction = FaceDirection::NONE;
    full = false;
    transparent = false;
    indexCount = 0;
    vertexCount = 0;
  }

  BlockFace(const BlockFace& face) {
    direction = face.direction;
    full = face.full;
    transparent = face.transparent;
    indexCount = face.indexCount;
    vertexCount = face.vertexCount;
    indices = new uint8_t[indexCount];
    memcpy(indices, face.indices, indexCount * sizeof(uint8_t));
    vertices = new float[(uint16_t)vertexCount * 3];
    memcpy(vertices, face.vertices, vertexCount * sizeof(float) * 3);
    uvs = new float[(uint16_t)vertexCount * 2];
    memcpy(uvs, face.uvs, vertexCount * sizeof(float) * 2);
  }

  ~BlockFace() {
    if (indices != nullptr) delete[] indices;
    if (vertices != nullptr) delete[] vertices;
    if (uvs != nullptr) delete[] uvs;
  }

  BlockFace& operator=(const BlockFace& face) {
    if (indices != nullptr) delete[] indices;
    if (vertices != nullptr) delete[] vertices;
    if (uvs != nullptr) delete[] uvs;
    direction = face.direction;
    full = face.full;
    transparent = face.transparent;
    indexCount = face.indexCount;
    vertexCount = face.vertexCount;
    indices = new uint8_t[indexCount];
    memcpy(indices, face.indices, indexCount * sizeof(uint8_t));
    vertices = new float[(uint16_t)vertexCount * 3];
    memcpy(vertices, face.vertices, vertexCount * sizeof(float) * 3);
    uvs = new float[(uint16_t)vertexCount * 2];
    memcpy(uvs, face.uvs, vertexCount * sizeof(float) * 2);
    return *this;
  }

  uint8_t* indices = nullptr;
  float* vertices = nullptr;
  float* uvs = nullptr;
  FaceDirection direction;
  uint8_t vertexCount;
  uint8_t indexCount;
  bool transparent;
  bool full;
};

#endif
