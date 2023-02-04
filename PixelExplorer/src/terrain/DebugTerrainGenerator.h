#ifndef PIXELEXPLORER_TERRAIN_DEBUG_TERRAIN_GENERATOR_H_
#define PIXELEXPLORER_TERRAIN_DEBUG_TERRAIN_GENERATOR_H_
#include "TerrainGenerator.h"

namespace pixelexplorer {
	namespace terrain {
		class DebugTerrainGenerator : public TerrainGenerator
		{
		public:
			DebugTerrainGenerator() = default;
			virtual ~DebugTerrainGenerator() = default;
			void generateTerrainData(TerrainChunk& chunk) override {
				if (chunk.getPosition().y == 0) {
					TerrainChunk::ChunkPoint* points = chunk.getPoints();
					for (uint32_t y = 0; y < TerrainChunk::CHUNK_GRID_SIZE; ++y) {
						for (uint32_t z = 0; z < TerrainChunk::CHUNK_GRID_SIZE; ++z) {
							for (uint32_t x = 0; x < TerrainChunk::CHUNK_GRID_SIZE; ++x) {
								if (x == z && x == y || x == 0 || z == 0)
									points[x + z * TerrainChunk::CHUNK_GRID_SIZE + y * TerrainChunk::CHUNK_LAYER_POINT_COUNT] = 1;
							}
						}
					}

					chunk.updateLastModified();
				}
				else if (chunk.getPosition().y < 0) {
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
#endif // !PIXELEXPLORER_TERRAIN_DEBUG_TERRAIN_GENERATOR_H_