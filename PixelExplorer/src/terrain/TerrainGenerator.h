#ifndef PIXELEXPLORER_TERRAIN_TERRAIN_GENERATOR_H_
#define PIXELEXPLORER_TERRAIN_TERRAIN_GENERATOR_H_
#include "PxeRefCount.h"
#include "TerrainChunk.h"

namespace pixelexplorer {
	namespace terrain {
		class TerrainGenerator : public pxengine::PxeRefCount
		{
		public:
			TerrainGenerator() = default;
			virtual ~TerrainGenerator() = default;
			virtual void generateTerrainData(TerrainChunk& chunk) = 0;
		};
	}
}
#endif // !PIXELEXPLORER_TERRAIN_TERRAIN_GENERATOR_H_