#include "ChunkManager.h"

#include <thread>

ChunkManager::ChunkManager(const char* chunkPath, Renderer* renderer)
    : _runningThreadCount(0), _killRunningThreads(false), _renderer(renderer) {
  _renderer->grab();
}

ChunkManager::~ChunkManager() {
  _killRunningThreads = true;
  _renderer->drop();
  unloadAllChunks();
  while (getRunningThreadCount() > 0)
    ;
}

void ChunkManager::unloadAllChunks() {
  for (const std::pair<std::string, Chunk*>& chunkPair : _chunkMap) chunkPair.second->drop();
  _chunkMap.clear();
}

void ChunkManager::unloadThreadFunction() {}

void ChunkManager::unloadChunk(glm::vec<3, int> pos) {}

void ChunkManager::loadChunk(glm::vec<3, int> pos) {
  Chunk* chunk = new Chunk(pos);
  chunk->generateChunk();
  chunk->updateMesh();
  _renderer->addMesh(chunk->getMesh());
  _chunkMap.insert({posToString(pos), chunk});
}
