#ifndef BLOCKFACE_H
#define BLOCKFACE_H
#include <cstdint>

#include "RefCounted.h"

enum class FaceDirection : uint8_t {
  NONE = 0,
  FRONT = 1,
  BACK = 2,
  LEFT = 3,
  RIGHT = 4,
  TOP = 5,
  BOTTOM = 6,
};

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
