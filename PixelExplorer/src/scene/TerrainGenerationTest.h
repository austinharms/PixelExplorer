#ifndef PIXELEXPLORER_TERRAIN_GENERATION_TEST_H_
#define PIXELEXPLORER_TERRAIN_GENERATION_TEST_H_
#include <stdint.h>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

#include "UpdatableScene.h"
#include "terrain/TerrainManager.h"
#include "terrain/TerrainRenderMesh.h"
#include "PxeAction.h"
#include "Camera.h"
#include "gui/PauseMenu.h"
#include "BS_thread_pool_light.hpp"
#include "tools/RenderLine.h"
#include "tools/RenderPoint.h"

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
			void updateInteractions();
			void updateTerrain();
			void jobUpdateTerrainLoading(const glm::i64vec3& currentLoadPos, const glm::i64vec3& lastLoadedPos);
			void jobLoadTerrain(const glm::i64vec3& terrainPos);
			// Note: calls drop on chunk due to using a ref caller must grab the terrain before passing it
			void jobUpdateTerrain(terrain::TerrainRenderMesh* terrainMesh);
			void jobUnloadTerrain(const glm::i64vec3& terrainPos);

			terrain::TerrainManager* _terrainManager;
			pxengine::PxeRenderMaterial* _terrainRenderMaterial;
			Camera* _camera;
			tools::RenderLine* _debugLine;
			tools::RenderPoint* _debugPoint;
			gui::PauseMenu* _pauseMenu;
			pxengine::PxeAction* _pauseAction;
			pxengine::PxeAction* _placeAction;
			pxengine::PxeAction* _breakAction;
			std::shared_mutex _terrainMutex;
			std::unordered_map<glm::i64vec3, terrain::TerrainRenderMesh*> _terrainChunks;
			BS::thread_pool_light _threadPool;
			glm::i64vec3 _lastLoadedChunkPosition;
			int64_t _loadDistance;
			bool _paused;
			bool _pauseHeld;
		};
	}
}
#endif