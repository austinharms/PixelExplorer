#include "World.h"

#include "Logger.h"
#include "WorldScene.h"
#include "rendering/TexturedMaterial.h"
#include "rendering/Shader.h"

namespace px::game::world {
World::World(rendering::Renderer* renderer) {
  _renderer = renderer;
  _renderer->grab();
  rendering::Shader* chunkShader = _renderer->loadShader("chunk");
  uint32_t texture = 0xffffffff;
  _chunkMaterial = new rendering::TexturedMaterial(chunkShader, &texture, 1, 1);
  chunkShader->drop();
  chunkShader = nullptr;
  _blockSet = new chunk::BlockSet();
  _scenes = new WorldScene*[1];
  _sceneCount = 1;
  _scenes[0] = new WorldScene(this);
  Logger::info("Loaded World");
}

World::~World() {
  _chunkMaterial->drop();

  for (uint8_t i = 0; i < _sceneCount; ++i) {
    if (!_scenes[i]->drop()) {
      Logger::warn("World Scene " + std::to_string(i) +
                   " still referenced after world destroyed");
    }
  }

  delete[] _scenes;

  if (!_blockSet->drop())
    Logger::warn("Block Set still referenced after world destroyed");
  Logger::info("Unloaded World");
}

void World::update() {
  for (uint8_t i = 0; i < _sceneCount; ++i) _scenes[i]->update();
}

chunk::ChunkRenderMesh* World::createChunkMesh() {
  return new chunk::ChunkRenderMesh(_chunkMaterial);
}
}  // namespace px::game::world
