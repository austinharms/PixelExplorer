#include "ChunkManager.h"

ChunkManager::ChunkManager() {}

ChunkManager::~ChunkManager() {}

void ChunkManager::UnloadChunks() {
  
}

void ChunkManager::LoadChunk(const glm::vec<3, int32_t> pos) {
  Chunk newChunk;
  newChunk.setPosition(pos);
  _chunks.emplace(pos, newChunk);
}
