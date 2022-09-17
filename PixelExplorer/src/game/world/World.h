#include "common/RefCount.h"

#include <filesystem>
#include <string>
#include <stdint.h>

#include "WorldDetails.h"
#include "../block/BlockManifest.h"
#include "../chunk/ChunkManager.h"
#include "engine/rendering/RenderWindow.h"

#ifndef PIXELEXPLORER_GAME_WORLD_WORLD_H_
#define PIXELEXPLORER_GAME_WORLD_WORLD_H_
namespace pixelexplorer::game::world {
	class World : public RefCount
	{
	public:
		World(WorldDetails& details, engine::rendering::RenderWindow& window);
		virtual ~World();
		// returns the ChunkManager associated with the dimensionId or null if there is not one
		chunk::ChunkManager* getChunkManager(uint32_t dimensionId) const;

	private:
		WorldDetails& _details;
		engine::rendering::RenderWindow& _window;
		block::BlockManifest& _blockManifest;
		chunk::ChunkManager** _dimensionChunkManagers;
		uint32_t _dimensionCount;
	};
}
#endif