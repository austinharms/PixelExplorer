#ifndef BLOCKFACE_H
#define BLOCKFACE_H
#include <cstdint>

#include "RefCounted.h"
#include "../FaceDirection.h"

struct BlockFace {
 public:
   BlockFace() { 
     indices = nullptr;
     vertices = nullptr;
     uvs = nullptr;
     direction = FaceDirection::NONE;
     solid = false;
     transparent = false;
     indexCount = 0;
     vertexCount = 0;
  }

  ~BlockFace() { 
    if (indices != nullptr)
      delete[] indices;
    if (vertices != nullptr)
      delete[] vertices;
    if (uvs != nullptr)
      delete[] uvs;
  }

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
