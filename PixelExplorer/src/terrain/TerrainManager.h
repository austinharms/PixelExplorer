#ifndef PIXELEXPLORER_TERRAIN_TERRAIN_MANAGER_H_
#define PIXELEXPLORER_TERRAIN_TERRAIN_MANAGER_H_
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

#include "TerrainChunk.h"
#include "TerrainGenerator.h"
#include "glm/gtx/hash.hpp"

namespace pixelexplorer {
	namespace terrain {
		class TerrainManager : public TerrainChunk::TerrainDropCallback
		{
		public:
			TerrainManager(TerrainGenerator& generator);
			~TerrainManager();
			TerrainGenerator& getTerrainGenerator();
			// Note: you do not need to grab the returned chunk but you must drop it
			TerrainChunk* grabTerrainChunk(const glm::i64vec3& position);
			bool getTerrainChunkLoaded(const glm::i64vec3& position);
			// Note: you do not need to grab the returned chunk but you must drop it
			TerrainChunk* grabLoadedTerrainChunk(const glm::i64vec3& position);

			// TerrainChunk::TerrainDropCallback
			void operator()(TerrainChunk& chunk) override;

		private:
			std::shared_mutex _terrainMutex;
			std::unordered_map<glm::i64vec3, TerrainChunk*> _loadedTerrain;
			TerrainGenerator* _terrainGenerator;
		};
	}
}
#endif // !PIXELEXPLORER_TERRAIN_MANAGER_H_
