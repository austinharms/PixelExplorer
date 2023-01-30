#ifndef PIXELEXPLORER_TERRAIN_GENERATION_TEST_H_
#define PIXELEXPLORER_TERRAIN_GENERATION_TEST_H_
#include <stdint.h>

#include "UpdatableScene.h"
#include "terrain/TerrainManager.h"
#include "terrain/TerrainRenderMesh.h"
#include "PxeAction.h"
#include "Camera.h"
#include "gui/PauseMenu.h"

namespace pixelexplorer {
	namespace scene {
		class TerrainGenerationTest : public UpdatableScene
		{
		public:
			TerrainGenerationTest();
			virtual ~TerrainGenerationTest();
			void update() override;
			void start(pxengine::PxeWindow& window) override;
			void stop() override;

		private:
			terrain::TerrainManager* _terrainManager;
			pxengine::PxeRenderMaterial* _terrainRenderMaterial;
			terrain::TerrainRenderMesh* _testTerrainMesh;
			Camera* _camera;
			gui::PauseMenu* _pauseMenu;
			pxengine::PxeAction* _pauseAction;
			bool _paused;
		};
	}
}
#endif