#pragma once
#include <cstdint>

#include "ChunkGenerator.h"
#include "../Block.h"
#include "glm/vec3.hpp"

class FlatChunkGenerator : public ChunkGenerator {
 public:
  FlatChunkGenerator(uint32_t blockId, int yHeight);
  ~FlatChunkGenerator();
  Block** genChunkBlocks(glm::vec<3, int> pos, Block** blocks);

 private:
  uint32_t _blockId;
  int _yHeight;
};