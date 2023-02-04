#ifndef PIXELEXPLORER_TERRAIN_FLAT_TERRAIN_GENERATOR_H_
#define PIXELEXPLORER_TERRAIN_FLAT_TERRAIN_GENERATOR_H_
#include "TerrainGenerator.h"

namespace pixelexplorer {
	namespace terrain {
		class FlatTerrainGenerator : public TerrainGenerator
		{
		public:
			FlatTerrainGenerator() = default;
			virtual ~FlatTerrainGenerator() = default;
			void generateTerrainData(TerrainChunk& chunk) override {
				if (chunk.getPosition().y <= 0) {
					TerrainChunk::ChunkPoint* points = chunk.getPoints();
					for (uint32_t i = 0; i < TerrainChunk::CHUNK_POINT_COUNT; ++i) {
						points[i] = 1;
					}

					chunk.updateLastModified();
				}
			}
		};
	}
}
#endif // !PIXELEXPLORER_TERRAIN_FLAT_TERRAIN_GENERATOR_H_