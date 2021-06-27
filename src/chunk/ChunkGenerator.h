#pragma once
#include <cstdint>

#include "Block.h"
#include "FastNoiseLite.h"
#include "RefCounted.h"
#include "glm/vec3.hpp"

class ChunkGenerator : public virtual RefCounted {
 public:
  ChunkGenerator(int seed);
  virtual ~ChunkGenerator();
  Block** genChunkBlocks(glm::vec<3, int> pos, Block** blocks);

 private:
  FastNoiseLite _baseNoise;
  FastNoiseLite _baseNoiseLayer;
  FastNoiseLite _paddingNoise;
  FastNoiseLite _tempatureNoise;
  uint32_t _topPaddingBlockId = 3;
  uint32_t _paddingBlockId = 2;
  uint32_t _defaultBlockId = 1;
};