#pragma once
#include <cstdint>

#include "ChunkGenerator.h"
#include "../block/Block.h"
#include "FastNoiseLite.h"
#include "RefCounted.h"
#include "glm/vec3.hpp"

class PerlinChunkGenerator : public ChunkGenerator {
 public:
  PerlinChunkGenerator(int seed);
  ~PerlinChunkGenerator();
  Block* genChunkBlocks(glm::vec<3, int> pos, Block* blocks);

 private:
  FastNoiseLite _baseNoise;
  FastNoiseLite _baseNoiseLayer;
  FastNoiseLite _paddingNoise;
  FastNoiseLite _tempatureNoise;
  uint32_t _topPaddingBlockId = 4;
  uint32_t _paddingBlockId = 3;
  uint32_t _defaultBlockId = 2;
};