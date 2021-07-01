#include "PerlinChunkGenerator.h"

#include "../Chunk.h"

PerlinChunkGenerator::PerlinChunkGenerator(int seed)
    : _baseNoise(seed),
      _baseNoiseLayer(seed + 1),
      _tempatureNoise(seed + 2),
      _paddingNoise(seed + 3) {
  _baseNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  _baseNoise.SetFractalType(FastNoiseLite::FractalType_None);
  _baseNoise.SetFrequency(0.0005);
  _baseNoiseLayer.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  _baseNoiseLayer.SetFractalType(FastNoiseLite::FractalType_None);
  _baseNoiseLayer.SetFrequency(0.002);
  _paddingNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  _paddingNoise.SetFractalType(FastNoiseLite::FractalType_None);
  _paddingNoise.SetFrequency(0.002);
  _tempatureNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  _tempatureNoise.SetFractalType(FastNoiseLite::FractalType_None);
  _tempatureNoise.SetFrequency(0.0007);
}

PerlinChunkGenerator::~PerlinChunkGenerator() {}

Block* PerlinChunkGenerator::genChunkBlocks(glm::vec<3, int> pos,
                                             Block* blocks) {
  for (int i = 0; i < Chunk::LAYER_SIZE; ++i) {
    float x = (i % Chunk::CHUNK_SIZE) + (pos.x * Chunk::CHUNK_SIZE);
    float z = ((i / Chunk::CHUNK_SIZE) % Chunk::CHUNK_SIZE) +
              (pos.z * Chunk::CHUNK_SIZE);
    int pillarHeight =
        (_baseNoise.GetNoise(x, z) + _baseNoiseLayer.GetNoise(x, z)) * 50;
    int padHeight = (_paddingNoise.GetNoise(x, z) + 1) * 10;
    for (int j = 0; j < Chunk::CHUNK_SIZE; ++j) {
      int y = (pos.y * Chunk::CHUNK_SIZE) + j;
      if (y > pillarHeight + padHeight) {
        blocks[j * Chunk::LAYER_SIZE + i] = Block(0);
      } else if (y == pillarHeight + padHeight) {
        blocks[j * Chunk::LAYER_SIZE + i] = Block(_topPaddingBlockId);
      } else if (y > pillarHeight) {
        blocks[j * Chunk::LAYER_SIZE + i] = Block(_paddingBlockId);
      } else {
        blocks[j * Chunk::LAYER_SIZE + i] = Block(_defaultBlockId);
      }
    }
  }

  return blocks;
}
