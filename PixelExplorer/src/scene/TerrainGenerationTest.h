#ifndef PIXELEXPLORER_TERRAIN_GENERATION_TEST_H_
#define PIXELEXPLORER_TERRAIN_GENERATION_TEST_H_
#include <stdint.h>

#include "UpdatableScene.h"
#include "terrain/TerrainManager.h"
#include "terrain/TerrainRenderMesh.h"

namespace pixelexplorer {
	namespace scene {
		class TerrainGenerationTest : public UpdatableScene
		{
		public:
			TerrainGenerationTest();
			virtual ~TerrainGenerationTest();
			void update() override;

		private:
			terrain::TerrainManager* _terrainManager;
			terrain::TerrainRenderMesh* _mesh;
		};
	}
}
#endif