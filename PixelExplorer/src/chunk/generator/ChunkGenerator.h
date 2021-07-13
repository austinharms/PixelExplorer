#pragma once
#include "../block/Block.h"
#include "RefCounted.h"
#include "glm/vec3.hpp"

class ChunkGenerator : public virtual RefCounted {
 public:
  virtual Block* genChunkBlocks(glm::vec<3, int> pos, Block* blocks) = 0;
};