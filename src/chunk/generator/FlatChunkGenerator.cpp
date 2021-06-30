#include "FlatChunkGenerator.h"

#include "../Chunk.h"

FlatChunkGenerator::FlatChunkGenerator(uint32_t blockId, int yHeight)
    : _blockId(blockId), _yHeight(yHeight) {}

FlatChunkGenerator::~FlatChunkGenerator() {}

Block** FlatChunkGenerator::genChunkBlocks(glm::vec<3, int> pos,
                                           Block** blocks) {
  for (int i = 0; i < Chunk::CHUNK_SIZE; ++i) {
    int endIndex = i * Chunk::LAYER_SIZE + Chunk::LAYER_SIZE;
    if (pos.y * Chunk::CHUNK_SIZE + i <= _yHeight) {
      for (int j = i * Chunk::LAYER_SIZE; j < endIndex; ++j)
        blocks[j] = new Block(_blockId);
    } else {
      for (int j = i * Chunk::LAYER_SIZE; j < endIndex; ++j)
        blocks[j] = nullptr;
    }
  }

  return blocks;
}
