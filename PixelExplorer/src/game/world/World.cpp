#include "World.h"

#include "Logger.h"
#include "WorldScene.h"
#include "rendering/Shader.h"
#include "rendering/TexturedMaterial.h"

namespace px::game::world {
World::World(rendering::Renderer* renderer) {
  _loaded = true;
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
  if (_loaded) unload();
}

void World::update() {
  if (!_loaded) return;
  for (uint8_t i = 0; i < _sceneCount; ++i) _scenes[i]->update();
}

void World::unload() {
  if (!_loaded) return;
  _loaded = false;
  _chunkMaterial->drop();
  _chunkMaterial = nullptr;

  _renderer->drop();
  _renderer = nullptr;

  for (uint8_t i = 0; i < _sceneCount; ++i) {
    _scenes[i]->unload();
    if (!_scenes[i]->drop()) {
      Logger::warn("World Scene " + std::to_string(i) +
                   " still referenced after world unloaded");
    }
  }

  delete[] _scenes;
  _scenes = nullptr;

  if (!_blockSet->drop())
    Logger::warn("Block Set still referenced after world unloaded");
  _blockSet = nullptr;
  Logger::info("Unloaded World");
}

bool World::getLoaded() const { return _loaded; }

chunk::ChunkRenderMesh* World::createChunkMesh() {
  if (!_loaded) return nullptr;
  chunk::ChunkRenderMesh* chunkMesh =
      new chunk::ChunkRenderMesh(_chunkMaterial);
  _renderer->addRenderable(chunkMesh);
  return chunkMesh;
}
}  // namespace px::game::world
