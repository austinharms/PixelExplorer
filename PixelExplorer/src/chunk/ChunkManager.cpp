#include "ChunkManager.h"

#include <iostream>
#include "physics/PhysicsManager.h"

ChunkManager::ChunkManager(Renderer* renderer) {
  _renderer = renderer;
  renderer->grab();
  _scene = PhysicsManager::CreateScene();
}

ChunkManager::~ChunkManager() {
  unloadChunks();
  _renderer->drop();
}

void ChunkManager::unloadChunks() {
  for (std::pair<const glm::vec<3, int32_t>, Chunk*>& pair : _chunks) {
    pair.second->setStatus(Chunk::Status::UNLOADED);
    pair.second->setRendererDropFlag(true);
    pair.second->unloadChunk();
    pair.second->drop();
  }

  _chunks.clear();
}

void ChunkManager::loadChunk(const glm::vec<3, int32_t> pos) {
  Chunk* newChunk = new Chunk();
  _chunks.insert({pos, newChunk});
  _renderer->addRenderable(newChunk);
  newChunk->setStatus(Chunk::Status::LOADING);
  newChunk->setPosition(pos);
  newChunk->setChunkManager(this);
  newChunk->setStatus(Chunk::Status::LOADED);
  newChunk->updateAdjacentChunks();
  //newChunk->updateMesh();
}

Chunk* ChunkManager::getChunk(glm::vec<3, int32_t> pos) {
  try {
    return _chunks.at(pos);
  } catch (...) {
    return nullptr;
  }
}

void ChunkManager::UpdateLoadedChunks() {
  for (auto& ch : _chunks) ch.second->updateMesh();
}
