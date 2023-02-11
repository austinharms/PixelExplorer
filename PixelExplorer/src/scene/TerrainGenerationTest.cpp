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
#include "PxScene.h"

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
			_placeAction = nullptr;
			_breakAction = nullptr;
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

			_placeAction = pxeGetEngine().getInputManager().getAction("Place");
			if (!_placeAction) {
				Application::Error("Out of Memory, Failed to create place action");
				return;
			}

			if (!_placeAction->hasSource()) {
				PxeActionSource* actionSrc = pxeGetEngine().getInputManager().getActionSource(((PxeActionSourceCode)PxeActionSourceType::KEYBOARD << 32) | SDLK_e);
				if (!actionSrc) {
					Application::Error("Out of Memory, Failed to create place action source");
					return;
				}

				_placeAction->addSource(*actionSrc);
				actionSrc->drop();
			}

			_breakAction = pxeGetEngine().getInputManager().getAction("Break");
			if (!_breakAction) {
				Application::Error("Out of Memory, Failed to create break action");
				return;
			}

			if (!_breakAction->hasSource()) {
				PxeActionSource* actionSrc = pxeGetEngine().getInputManager().getActionSource(((PxeActionSourceCode)PxeActionSourceType::KEYBOARD << 32) | SDLK_q);
				if (!actionSrc) {
					Application::Error("Out of Memory, Failed to create break action source");
					return;
				}

				_breakAction->addSource(*actionSrc);
				actionSrc->drop();
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
				updateInteractions();
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

		void TerrainGenerationTest::updateInteractions()
		{
			using namespace physx;
			using namespace terrain;
			PxScene* scene = getScene()->getPhysicsScene();
			const glm::vec3& camPos = _camera->getPosition();
			const glm::vec3& camDir = _camera->getForward();
			PxVec3 rayStart(camPos.x, camPos.y, camPos.z);
			PxVec3 rayDir(camDir.x, camDir.y, camDir.z);
			PxRaycastBuffer rayRes;
			scene->lockRead();
			bool hit = scene->raycast(rayStart, rayDir, 1000, rayRes);
			scene->unlockRead();
			if (hit && rayRes.hasBlock) {
				const glm::vec3 hitPos(rayRes.block.position.x, rayRes.block.position.y, rayRes.block.position.z);
				if (_placeAction->getValue()) {
					const glm::i64vec3 chunkSpace(TerrainChunk::WorldToChunkSpace(hitPos - camDir * TerrainChunk::CHUNK_CELL_SIZE));
					TerrainChunk* terrainChunk = _terrainManager->getTerrainChunk(TerrainChunk::ChunkSpaceToChunkPosition(chunkSpace));
					uint32_t pointIndex = TerrainChunk::RelativeChunkSpaceToPointIndex(TerrainChunk::ChunkSpaceToRelativeChunkSpace(chunkSpace));
					if (terrainChunk->getPoints()[pointIndex] != 1) {
						terrainChunk->getPoints()[pointIndex] = 1;
						terrainChunk->updateLastModified();
					}

					terrainChunk->drop();
				}
				else if (_breakAction->getValue()) {
					const glm::i64vec3 chunkSpace(TerrainChunk::WorldToChunkSpace(hitPos + camDir * TerrainChunk::CHUNK_CELL_SIZE));
					TerrainChunk* terrainChunk = _terrainManager->getTerrainChunk(TerrainChunk::ChunkSpaceToChunkPosition(chunkSpace));
					uint32_t pointIndex = TerrainChunk::RelativeChunkSpaceToPointIndex(TerrainChunk::ChunkSpaceToRelativeChunkSpace(chunkSpace));
					if (terrainChunk->getPoints()[pointIndex] != 0) {
						terrainChunk->getPoints()[pointIndex] = 0;
						terrainChunk->updateLastModified();
					}

					terrainChunk->updateLastModified();
					terrainChunk->drop();
				}

				//PEX_INFO(("Ray Hit: x:" + std::to_string(rayRes.block.position.x) + ", y: " + std::to_string(rayRes.block.position.y) + ", z: " + std::to_string(rayRes.block.position.z)).c_str());
			}

		}

		void TerrainGenerationTest::updateTerrain()
		{
			using namespace terrain;
			glm::i64vec3 cameraChunkPos = TerrainChunk::ChunkSpaceToChunkPosition(TerrainChunk::WorldToChunkSpace(_camera->getPosition()));
			if (llabs(cameraChunkPos.x - _lastLoadedChunkPosition.x) >= 2 ||
				llabs(cameraChunkPos.y - _lastLoadedChunkPosition.y) >= 2 ||
				llabs(cameraChunkPos.z - _lastLoadedChunkPosition.z) >= 2)
			{
				_threadPool.push_task(&TerrainGenerationTest::jobUpdateTerrainLoading, this, cameraChunkPos, _lastLoadedChunkPosition);
				_lastLoadedChunkPosition = cameraChunkPos;
			}

			constexpr uint8_t MAX_REFINE_MESH_COUNT = 5;
			uint8_t refineCount = 0;
			TerrainRenderMesh* refineMeshQueue[MAX_REFINE_MESH_COUNT];

			{
				std::shared_lock lock(_terrainMutex);
				for (auto pair : _terrainChunks) {
					TerrainRenderMesh* mesh = pair.second;
					if (!mesh->getWorkPending()) {
						if (mesh->getMeshOutdated()) {
							mesh->setWorkPending();
							mesh->grab();
							_threadPool.push_task_priority(&TerrainGenerationTest::jobUpdateTerrain, this, pair.second);
						}
						else if (mesh->getWorkingMeshType() == TerrainRenderMesh::FAST && refineCount < MAX_REFINE_MESH_COUNT) {
							mesh->grab();
							refineMeshQueue[refineCount++] = mesh;
						}
					}
				}
			}

			if (refineCount) {
				if (_threadPool.get_tasks_queued() < _threadPool.get_thread_count()) {
					for (uint8_t i = 0; i < refineCount; ++i) {
						refineMeshQueue[i]->setWorkPending();
						_threadPool.push_task(&TerrainGenerationTest::jobUpdateTerrain, this, refineMeshQueue[i]);
					}
				}
				else {
					for (uint8_t i = 0; i < refineCount; ++i) {
						refineMeshQueue[i]->drop();
					}
				}
			}
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
			if (terrainMesh->getMeshOutdated()) {
				terrainMesh->rebuildMesh(terrain::TerrainRenderMesh::FAST);
			}
			else if (terrainMesh->getWorkingMeshType() == terrain::TerrainRenderMesh::FAST) {
				terrainMesh->rebuildMesh(terrain::TerrainRenderMesh::REFINED);
			}
			else {
				PEX_WARN("Updated terrain chunk with no updates");
				terrainMesh->resetWorkPending();
			}

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
			terrainMesh->setWorkPending();
			_terrainMutex.lock();
			bool inserted = _terrainChunks.emplace(terrainPos, terrainMesh).second;
			_terrainMutex.unlock();

			if (inserted) {
				getScene()->addObject(*terrainMesh);
				terrainMesh->rebuildMesh(terrain::TerrainRenderMesh::REFINED);
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
