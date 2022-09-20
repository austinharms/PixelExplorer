#include "World.h"

#include "../chunk/ChunkGenerator.h"
#include "../chunk/FlatChunkGenerator.h"

namespace pixelexplorer::game::world {
	World::World(WorldDetails& details, engine::rendering::RenderWindow& window) : _details(details), _window(window) {
		_details.grab();
		_window.grab();

		_blockManifest = new(std::nothrow) block::BlockManifest(_details.Path);
		if (_blockManifest == nullptr)
			Logger::fatal(__FUNCTION__" failed to allocate world BlockManifest");

		_renderMeshFactory = new(std::nothrow) chunk::ChunkRenderMeshFactory(_window);
		if (_renderMeshFactory == nullptr)
			Logger::fatal(__FUNCTION__" failed to allocate world ChunkRenderMeshFactory");

		chunk::ChunkGenerator* generator = new(std::nothrow) chunk::FlatChunkGenerator(_blockManifest->getBlockId("PX_TEST_BLOCK"), 10);
		if (generator == nullptr)
			Logger::fatal(__FUNCTION__" failed to allocate world ChunkGenerator");

		_dimensionCount = 1;
		_dimensionChunkManagers = (chunk::ChunkManager**)calloc(_dimensionCount, sizeof(chunk::ChunkManager*));
		if (_dimensionChunkManagers == nullptr)
			Logger::fatal(__FUNCTION__" failed to allocate world ChunkManager array");
		for (uint32_t i = 0; i < _dimensionCount; ++i) {
			_dimensionChunkManagers[i] = new(std::nothrow) chunk::ChunkManager(*_renderMeshFactory, *generator, *_blockManifest);
			if (_dimensionChunkManagers[i] == nullptr)
				Logger::fatal(__FUNCTION__" failed to allocate world ChunkManager");
		}

		generator->drop();
		generator = nullptr;

		_player = new(std::nothrow) player::Player(glm::vec3(0, 0, -20), window.getInputManager());
		if (_player == nullptr)
			Logger::fatal(__FUNCTION__" failed to allocate world Player");
	}

	World::~World() {
		for (uint32_t i = 0; i < _dimensionCount; ++i) {
			if (!_dimensionChunkManagers[i]->drop())
				Logger::warn(__FUNCTION__" world ChunkManager not dropped, make sure all other references to the ChunkManager are dropped");
			_dimensionChunkManagers[i] = nullptr;
		}

		free(_dimensionChunkManagers);

		if (!_player->drop())
			Logger::warn(__FUNCTION__" world Player not dropped, make sure all other references to the Player are dropped");

		if (!_renderMeshFactory->drop())
			Logger::warn(__FUNCTION__" world ChunkRenderMeshFactory not dropped, make sure all other references to the ChunkRenderMeshFactory are dropped");

		if (!_blockManifest->drop())
			Logger::warn(__FUNCTION__" world BlockManifest not dropped, make sure all other references to the BlockManifest are dropped");

		_details.drop();
		_window.drop();
	}

	chunk::ChunkManager* World::getChunkManager(uint32_t dimensionId) const
	{
		if (dimensionId >= _dimensionCount || _dimensionChunkManagers == nullptr)	return nullptr;
		return _dimensionChunkManagers[dimensionId];
	}

	block::BlockManifest& game::world::World::getBlockManifest() const
	{
		return *_blockManifest;
	}

	player::Player& game::world::World::getPlayer() const
	{
		return *_player;
	}
}