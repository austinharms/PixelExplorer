#include "WorldScene.h"

#include "Logger.h"
#include "World.h"
#include "chunk/Chunk.h"

namespace px::game::world {
	px::game::world::WorldScene::WorldScene(World* world) {
		_loaded = true;
		world->grab();
		_world = world;
		_physicsScene = new physics::PhysicsScene();
		for (int32_t x = 0; x < 25; ++x) {
			for (int32_t y = 0; y < 25; ++y) {
				for (int32_t z = 0; z < 25; ++z) {
					chunk::Chunk* chunk = new chunk::Chunk(_world->createChunkMesh());
					chunk->setPosition(glm::ivec3(x, y, z));
					chunk->setScene(this);
					chunk->updateAdjacents();
					chunk->updateMesh();
					_loadedChunks.emplace(chunk->getPosition(), chunk);
				}
			}
		}
	}

	WorldScene::~WorldScene() {
		if (_loaded) unload();
	}

	void WorldScene::update() {}

	void WorldScene::unload() {
		if (!_loaded) return;
		_loaded = false;
		_world->drop();
		_world = nullptr;

		for (auto it = _loadedChunks.begin(); it != _loadedChunks.end(); ++it) {
			it->second->unload();
			if (!(it->second->drop())) {
				Logger::warn("Chunk still referenced after scene unloaded");
			}
		}

		if (!_physicsScene->drop()) {
			Logger::warn("Physics scene still referenced after scene unloaded");
		}
	}

	bool WorldScene::getLoaded() const { return _loaded; }
}  // namespace px::game::world