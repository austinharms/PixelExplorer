#include "WorldScene.h"

#include "Logger.h"
#include "World.h"
#include "chunk/Chunk.h"

namespace px::game::world {
px::game::world::WorldScene::WorldScene(World* world) {
  world->grab();
  _world = world;
  _physicsScene = new physics::PhysicsScene();
  chunk::Chunk* chunk = new chunk::Chunk(_world->createChunkMesh());
  chunk->setPosition(glm::ivec3(0, 0, 0));
  chunk->setScene(this);
  chunk->updateAdjacents();
  chunk->updateMesh();
  _loadedChunks.emplace(chunk->getPosition(), chunk);
}

WorldScene::~WorldScene() {
  for (auto it = _loadedChunks.begin(); it != _loadedChunks.end(); ++it) {
    it->second->unload();
    if (!(it->second->drop())) {
      Logger::warn("Chunk still referenced after scene destroyed");
    }
  }

  if (!_physicsScene->drop()) {
    Logger::warn("Physics scene still referenced after scene destroyed");
  }

  _world->drop();
}

void WorldScene::update() {}
}  // namespace px::game::world