#ifndef BLOCKFACE_H
#define BLOCKFACE_H
#include <cstdint>

#include "RefCounted.h"
#include "../FaceDirection.h"

struct BlockFace : public virtual RefCounted {
 public:
  uint8_t* indices;
  float* vertices;
  float* uvs;
  FaceDirection direction;
  uint8_t vertexCount;
  uint8_t indexCount;
  bool transparent;
  bool solid;
};

#endif
