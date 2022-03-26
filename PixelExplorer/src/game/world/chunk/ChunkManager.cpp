#include "ChunkManager.h"

ChunkManager::ChunkManager(Renderer* renderer) {}

ChunkManager::~ChunkManager() {}

void ChunkManager::UnloadChunks() {}

void ChunkManager::LoadChunk(glm::vec<3, int32_t> pos) {}

Chunk* ChunkManager::GetChunk(glm::vec<3, int32_t> pos) { return nullptr; }

void ChunkManager::UpdateLoadedChunks() {}
