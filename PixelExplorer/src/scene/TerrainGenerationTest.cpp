#include "TerrainGenerationTest.h"

#include <new>
#include <math.h>

#include "Application.h"
#include "terrain/TerrainGenerator.h"
#include "terrain/DebugTerrainGenerator.h"
#include "PxeEngine.h"
#include "PxeRenderMaterial.h"
#include "PxeFSHelpers.h"
#include "Log.h"
#include "PxeActionSource.h"
#include "PxeInputManager.h"
#include "SDL_keycode.h"
#include "terrain/TerrainChunk.h"
#include "SDL_timer.h"

namespace pixelexplorer {
	namespace scene {
		TerrainGenerationTest::TerrainGenerationTest() : _threadPool((std::thread::hardware_concurrency() - 4) > 0 ? (std::thread::hardware_concurrency() - 4) : 2)
		{
			using namespace terrain;
			using namespace pxengine;
			_terrainManager = nullptr;
			_terrainRenderMaterial = nullptr;
			_camera = nullptr;
			_pauseAction = nullptr;
			_pauseMenu = nullptr;
			// Dummy value that is well outside the position of the camera to force terrain reloading
			_lastLoadedChunkPosition = glm::i64vec3(-10000, -10000, -10000);
			_loadDistance = 7;
			_paused = false;
			_pauseHeld = false;

			_pauseAction = pxeGetEngine().getInputManager().getAction("Pause");
			if (!_pauseAction) {
				Application::Error("Out of Memory, Failed to create pause action");
				return;
			}

			if (!_pauseAction->hasSource()) {
				PxeActionSource* pauseScr = pxeGetEngine().getInputManager().getActionSource(((PxeActionSourceCode)PxeActionSourceType::KEYBOARD << 32) | SDLK_ESCAPE);
				if (!pauseScr) {
					Application::Error("Out of Memory, Failed to create pause action source");
					return;
				}

				_pauseAction->addSource(*pauseScr);
				pauseScr->drop();
			}

			_pauseMenu = new(std::nothrow) gui::PauseMenu();
			if (!_pauseMenu) {
				Application::Error("Out of Memory, Failed to allocate Pause Menu");
				return;
			}

			TerrainGenerator* gen = new(std::nothrow) DebugTerrainGenerator();
			if (!gen) {
				Application::Error("Out of Memory, Failed to create Terrain Generator");
				return;
			}

			_terrainManager = new(std::nothrow) TerrainManager(*gen);
			gen->drop();
			gen = nullptr;
			if (!_terrainManager) {
				Application::Error("Out of Memory, Failed to create Terrain Manager");
				return;
			}

			PxeShader* shader = pxeGetEngine().loadShader(getAssetPath("shaders") / "terrain.pxeshader");
			if (!shader) {
				Application::Error("Failed to load terrain shader");
				return;
			}

			_terrainRenderMaterial = new(std::nothrow) PxeRenderMaterial(*shader);
			shader->drop();
			if (!_terrainRenderMaterial) {
				Application::Error("Out of Memory, Failed to create terrain Render Material");
				return;
			}
		}

		TerrainGenerationTest::~TerrainGenerationTest()
		{
			_threadPool.wait_for_tasks();
			if (_pauseMenu)
				_pauseMenu->drop();

			if (_pauseAction)
				_pauseAction->drop();

			_terrainMutex.lock();
			for (auto meshPair : _terrainChunks) {
				if (meshPair.second) {
					meshPair.second->unloadChunks();
					meshPair.second->drop();
				}
			}

			_terrainChunks.clear();
			_terrainMutex.unlock();

			if (_terrainRenderMaterial)
				_terrainRenderMaterial->drop();

			if (_terrainManager)
				delete _terrainManager;
		}

		void TerrainGenerationTest::update()
		{
			if (_pauseHeld && !_pauseAction->getValue())
				_pauseHeld = false;

			if (_paused) {
				if (_pauseMenu->getActions() & gui::PauseMenu::PLAY || _pauseAction->getValue() && !_pauseHeld) {
					_pauseHeld = true;
					_paused = false;
					getScene()->removeObject(*_pauseMenu);
					_camera->lockCursor();
				}
			}
			else {
				if (_pauseAction->getValue() && !_pauseHeld) {
					_paused = true;
					_pauseHeld = true;
					getScene()->addObject(*_pauseMenu);
					_camera->unlockCursor();
					return;
				}

				_camera->update();
				updateTerrain();
			}
		}

		void TerrainGenerationTest::start(pxengine::PxeWindow& window)
		{
			_camera = new(std::nothrow) Camera(window, glm::radians(90.0f), 0.1f, 1000.0f);
			if (!_camera) {
				Application::Error("Out of Memory, Failed to allocate Camera");
				return;
			}

			if (_camera->getErrorFlag()) {
				Application::Error("Failed to create Camera");
				return;
			}

			_camera->lockCursor();
		}

		void TerrainGenerationTest::stop()
		{
			if (_camera) {
				_camera->unlockCursor();
				_camera->drop();
				_camera = nullptr;
			}
		}

		void TerrainGenerationTest::updateTerrain()
		{
			using namespace terrain;
			glm::i64vec3 cameraChunkPos = TerrainChunk::ChunkSpaceToChunkPosition(TerrainChunk::WorldToChunkSpace(_camera->getPosition()));
			std::future<void> requiredUpdates[27];
			{
				glm::i64vec3 loadPos(-2, -1, -1);
				for (int8_t i = 0; i < 27; ++i) {
					if (++loadPos.x == 2) {
						loadPos.x = -1;
						if (++loadPos.z == 2) {
							loadPos.z = -1;
							++loadPos.y;
						}
					}

					requiredUpdates[i] = _threadPool.submit_priority(&TerrainGenerationTest::jobLoadTerrain, this, cameraChunkPos + loadPos);
				}
			}

			if (llabs(cameraChunkPos.x - _lastLoadedChunkPosition.x) >= 2 ||
				llabs(cameraChunkPos.y - _lastLoadedChunkPosition.y) >= 2 ||
				llabs(cameraChunkPos.z - _lastLoadedChunkPosition.z) >= 2)
			{
				_threadPool.push_task(&TerrainGenerationTest::jobUpdateTerrainLoading, this, cameraChunkPos, _lastLoadedChunkPosition);
				_lastLoadedChunkPosition = cameraChunkPos;
			}

			{
				std::shared_lock lock(_terrainMutex);
				for (auto pair : _terrainChunks) {
					if (pair.second->meshRebuildRequired()) {
						pair.second->grab();
						_threadPool.push_task(&TerrainGenerationTest::jobUpdateTerrain, this, pair.second);
					}
				}
			}

			for (int8_t i = 0; i < 27; ++i)
				requiredUpdates[i].wait();
		}

		void TerrainGenerationTest::jobUpdateTerrainLoading(const glm::i64vec3& currentLoadPos, const glm::i64vec3& lastLoadedPos) {
			for (int64_t x = -_loadDistance; x < _loadDistance + 1; ++x) {
				for (int64_t y = -_loadDistance; y < _loadDistance + 1; ++y) {
					for (int64_t z = -_loadDistance; z < _loadDistance + 1; ++z) {
						glm::i64vec3 loopPos(x, y, z);
						glm::i64vec3 unloadPos = loopPos + lastLoadedPos;
						glm::i64vec3 loadPos = loopPos + currentLoadPos;

						// Check if unload position is out of loaded area
						if (llabs(unloadPos.x - currentLoadPos.x) > _loadDistance ||
							llabs(unloadPos.y - currentLoadPos.y) > _loadDistance ||
							llabs(unloadPos.z - currentLoadPos.z) > _loadDistance)
						{
							_threadPool.push_task(&TerrainGenerationTest::jobUnloadTerrain, this, unloadPos);
						}

						// Check if load position was already loaded
						if (llabs(loadPos.x - lastLoadedPos.x) > _loadDistance ||
							llabs(loadPos.y - lastLoadedPos.y) > _loadDistance ||
							llabs(loadPos.z - lastLoadedPos.z) > _loadDistance)
						{
							_threadPool.push_task(&TerrainGenerationTest::jobLoadTerrain, this, loadPos);
						}
					}
				}
			}
		}

		void TerrainGenerationTest::jobUpdateTerrain(terrain::TerrainRenderMesh* terrainMesh) {
			if (terrainMesh->meshRebuildRequired())
				terrainMesh->rebuildMesh();
			terrainMesh->drop();
		}

		void TerrainGenerationTest::jobLoadTerrain(const glm::i64vec3& terrainPos)
		{
			using namespace terrain;
			{
				std::shared_lock lock(_terrainMutex);
				auto terrainItr = _terrainChunks.find(terrainPos);
				if (terrainItr != _terrainChunks.end()) return;
			}

			TerrainRenderMesh* terrainMesh = new(std::nothrow) TerrainRenderMesh(*_terrainRenderMaterial);
			if (!terrainMesh) {
				Application::Error("Out of Memory, Failed to create Terrain Render Mesh");
				return;
			}

			terrainMesh->loadChunks(terrainPos, *_terrainManager);
			terrainMesh->grab();
			_terrainMutex.lock();
			bool inserted = _terrainChunks.emplace(terrainPos, terrainMesh).second;
			_terrainMutex.unlock();

			if (inserted) {
				getScene()->addObject(*terrainMesh);
				terrainMesh->rebuildMesh();
			}
			else {
				PEX_WARN(("Attempted to load terrain that was already loaded, x: " + std::to_string(terrainPos.x) + " y: " + std::to_string(terrainPos.y) + " z: " + std::to_string(terrainPos.z)).c_str());
				terrainMesh->unloadChunks();
				terrainMesh->drop();
			}

			terrainMesh->drop();
		}

		void TerrainGenerationTest::jobUnloadTerrain(const glm::i64vec3& terrainPos)
		{
			using namespace terrain;
			TerrainRenderMesh* terrainMesh;
			{
				std::unique_lock lock(_terrainMutex);
				auto terrainItr = _terrainChunks.find(terrainPos);
				if (terrainItr == _terrainChunks.end()) return;
				terrainMesh = terrainItr->second;
				_terrainChunks.erase(terrainItr);
			}

			terrainMesh->unloadChunks();
			getScene()->removeObject(*terrainMesh);
			terrainMesh->drop();
		}
	}
}
