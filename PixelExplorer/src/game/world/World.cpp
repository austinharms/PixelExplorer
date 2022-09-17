#include "World.h"

namespace pixelexplorer::game::world {
	World::World(WorldDetails& details, engine::rendering::RenderWindow& window) : _details(details), _window(window), _blockManifest(*(new block::BlockManifest(_details.Path))) {
		_details.grab();
		_window.grab();
		_dimensionCount = 1;
		_dimensionChunkManagers = (chunk::ChunkManager**)calloc(_dimensionCount, sizeof(chunk::ChunkManager*));
		if (_dimensionChunkManagers != nullptr) {
			for (uint32_t i = 0; i < _dimensionCount; ++i)
				_dimensionChunkManagers[i] = new chunk::ChunkManager(_window, _blockManifest);
		}
		else {
			Logger::error(__FUNCTION__" failed to allocate world dimension ChunkManagers array");
		}
	}

	World::~World() {
		for (uint32_t i = 0; i < _dimensionCount; ++i) {
			if (!_dimensionChunkManagers[i]->drop()) {
				Logger::warn(__FUNCTION__" World ChunkManager not dropped, make sure all other references to the ChunkManager are dropped");
			}

			_dimensionChunkManagers[i] = nullptr;
		}

		free(_dimensionChunkManagers);
		_blockManifest.drop();
		_details.drop();
		_window.drop();
	}

	chunk::ChunkManager* World::getChunkManager(uint32_t dimensionId) const
	{
		if (dimensionId >= _dimensionCount || _dimensionChunkManagers == nullptr)	return nullptr;
		return _dimensionChunkManagers[dimensionId];
	}
}