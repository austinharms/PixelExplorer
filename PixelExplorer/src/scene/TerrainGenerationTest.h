#ifndef PIXELEXPLORER_TERRAIN_GENERATION_TEST_H_
#define PIXELEXPLORER_TERRAIN_GENERATION_TEST_H_
#include <stdint.h>
#include <unordered_map>

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
			std::unordered_map<glm::i64vec3, terrain::TerrainRenderMesh*> _terrainChunks;
			Camera* _camera;
			gui::PauseMenu* _pauseMenu;
			pxengine::PxeAction* _pauseAction;
			bool _paused;
			bool _pauseHeld;
		};
	}
}
#endif