#include "World.h"

namespace pixelexplorer::game::world {
	World::World(WorldDetails& details, engine::rendering::RenderWindow& window) : _details(details), _window(window), _blockManifest(*(new block::BlockManifest(_details.Path))) {
		_details.grab();
		_window.grab();
		_dimensionCount = 1;
		_dimensionChunkManagers = (chunk::ChunkManager**)calloc(_dimensionCount, sizeof(chunk::ChunkManager*));
		for (uint32_t i = 0; i < _dimensionCount; ++i)
			_dimensionChunkManagers[i] = new chunk::ChunkManager();
	}

	World::~World() {
		_blockManifest.drop();
		_details.drop();
	}
}