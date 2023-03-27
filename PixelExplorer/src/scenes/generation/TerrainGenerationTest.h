#ifndef PIXELEXPLORER_TERRAIN_GENERATION_TEST_H_
#define PIXELEXPLORER_TERRAIN_GENERATION_TEST_H_
#include <stdint.h>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

#include "ApplicationScene.h"
#include "terrain/TerrainManager.h"
#include "terrain/rendering/TerrainRenderProperties.h"
#include "terrain/TerrainObject.h"
#include "PxeAction.h"
#include "Camera.h"
#include "PauseMenu.h"
#include "BS_thread_pool_light.hpp"

namespace pixelexplorer {
	namespace scenes {
		namespace generation {
			class TerrainGenerationTest : public ApplicationScene
			{
			public:
				TerrainGenerationTest();
				virtual ~TerrainGenerationTest();
				void onUpdate() override;
				void onStart(pxengine::PxeWindow& window) override;
				void onStop() override;

			private:
				void updateInteractions();
				void updateTerrain();
				void jobUpdateTerrainLoading(const glm::i64vec3& currentLoadPos, const glm::i64vec3& lastLoadedPos);
				void jobLoadTerrain(const glm::i64vec3& terrainPos);
				// Note: calls drop on chunk due to using a ref caller must grab the terrain before passing it
				void jobUpdateTerrain(terrain::TerrainObject* terrain);
				void jobUnloadTerrain(const glm::i64vec3& terrainPos);

				terrain::TerrainManager* _terrainManager;
				terrain::TerrainRenderProperties* _terrainRenderProperties;
				Camera* _camera;
				PauseMenu* _pauseMenu;
				pxengine::PxeAction* _pauseAction;
				pxengine::PxeAction* _placeAction;
				pxengine::PxeAction* _breakAction;
				std::shared_mutex _terrainMutex;
				std::unordered_map<glm::i64vec3, terrain::TerrainObject*> _terrainObjects;
				BS::thread_pool_light _threadPool;
				glm::i64vec3 _lastLoadedChunkPosition;
				int64_t _loadDistance;
				bool _paused;
				bool _pauseHeld;
			};
		}
	}
}
#endif