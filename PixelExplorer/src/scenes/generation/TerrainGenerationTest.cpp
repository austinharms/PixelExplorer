#include "TerrainGenerationTest.h"

#include <new>
#include <math.h>

#include "Application.h"
#include "terrain/TerrainGenerator.h"
#include "terrain/DebugTerrainGenerator.h"
#include "PxeEngine.h"
#include "PxeFSHelpers.h"
#include "Log.h"
#include "PxeActionSource.h"
#include "PxeInputManager.h"
#include "SDL_keycode.h"
#include "terrain/TerrainChunk.h"
#include "SDL_timer.h"
#include "PxScene.h"
#include "PxeRenderPipeline.h"
#include "PxeInputManager.h"

namespace pixelexplorer {
	namespace scenes {
		namespace generation {
			TerrainGenerationTest::TerrainGenerationTest() : _threadPool((std::thread::hardware_concurrency() - 4) > 0 ? (std::thread::hardware_concurrency() - 4) : 2)
			{
				using namespace terrain;
				using namespace pxengine;
				_terrainManager = nullptr;
				_terrainRenderProperties = nullptr;
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

				PxeEngine& engine = PxeEngine::getInstance();
				PxeInputManager& inputManager = engine.getInputManager();
				PxeRenderPipeline& renderPipeline = engine.getRenderPipeline();

				_pauseAction = inputManager.getAction("Pause");
				if (!_pauseAction) { PEX_FATAL("Failed to create pause PxeAction"); }

				if (!_pauseAction->hasSource()) {
					PxeActionSource* pauseScr = inputManager.getActionSource(PxeKeyboardActionSourceCode(SDLK_ESCAPE));
					if (!pauseScr) { PEX_FATAL("Failed to create pause PxeActionSource"); }
					_pauseAction->addSource(*pauseScr);
					pauseScr->drop();
				}

				_placeAction = inputManager.getAction("Place");
				if (!_placeAction) { PEX_FATAL("Failed to create place PxeAction"); }

				if (!_placeAction->hasSource()) {
					PxeActionSource* actionSrc = inputManager.getActionSource(PxeKeyboardActionSourceCode(SDLK_e));
					if (!actionSrc) { PEX_FATAL("Failed to create place PxeActionSource"); }
					_placeAction->addSource(*actionSrc);
					actionSrc->drop();
				}

				_breakAction = inputManager.getAction("Break");
				if (!_breakAction) { PEX_FATAL("Failed to create break PxeAction"); }

				if (!_breakAction->hasSource()) {
					PxeActionSource* actionSrc = inputManager.getActionSource(PxeKeyboardActionSourceCode(SDLK_q));
					if (!actionSrc) { PEX_FATAL("Failed to create break PxeActionSource"); }
					_breakAction->addSource(*actionSrc);
					actionSrc->drop();
				}

				PxeObject* obj = PxeObject::create();
				if (!obj) { PEX_FATAL("Failed to create PauseMenu PxeObject"); }

				_pauseMenu = new(std::nothrow) PauseMenu();
				if (!_pauseMenu) { PEX_FATAL("Failed to allocate TerrainGenerationTest's PauseMenu"); }
				_pauseMenu->setEnabled(false);
				if (!obj->addComponent(*_pauseMenu)) {
					PEX_FATAL("Failed to add PauseMenu to PxeObject");
				}

				if (!getScene().addObject(*obj)) {
					PEX_FATAL("Failed to add PauseMenu PxeObject to TerrainGenerationTest's PxeScene");
				}

				obj->drop();


				TerrainGenerator* gen = new(std::nothrow) DebugTerrainGenerator();
				if (!gen) { PEX_FATAL("Failed to allocate TerrainGenerationTest's TerrainGenerator"); }
				_terrainManager = new(std::nothrow) TerrainManager(*gen);
				gen->drop();
				if (!_terrainManager) { PEX_FATAL("Failed to allocate TerrainGenerationTest's TerrainManager"); }


				_terrainRenderProperties = new(std::nothrow) TerrainRenderProperties();
				if (!_terrainRenderProperties) { PEX_FATAL("Failed to allocate TerrainGenerationTest's TerrainRenderProperties"); }
				_terrainRenderProperties->setLightDirection(glm::normalize(glm::vec3(1, -2, 1)));
			}

			TerrainGenerationTest::~TerrainGenerationTest()
			{
				_threadPool.wait_for_tasks();
				if (_pauseMenu)
					_pauseMenu->drop();

				if (_pauseAction)
					_pauseAction->drop();

				_terrainMutex.lock();
				for (auto objPair : _terrainObjects) {
					if (objPair.second) {
						objPair.second->unloadTerrainChunk();
						objPair.second->drop();
					}
				}

				_terrainObjects.clear();
				_terrainMutex.unlock();

				if (_terrainRenderProperties)
					_terrainRenderProperties->drop();

				if (_terrainManager)
					delete _terrainManager;
			}

			void TerrainGenerationTest::onUpdate()
			{
				if (_pauseHeld && !_pauseAction->getValue())
					_pauseHeld = false;

				if (_paused) {
					if (_pauseMenu->getActions() & PauseMenu::PLAY || _pauseAction->getValue() && !_pauseHeld) {
						_pauseHeld = true;
						_paused = false;
						_pauseMenu->setEnabled(false);
						_camera->lockCursor();
					}
				}
				else {
					if (_pauseAction->getValue() && !_pauseHeld) {
						_paused = true;
						_pauseHeld = true;
						_pauseMenu->setEnabled(true);
						_camera->unlockCursor();
						return;
					}

					_camera->update();
					updateInteractions();
					updateTerrain();
				}
			}

			void TerrainGenerationTest::onStart(pxengine::PxeWindow& window)
			{
				_camera = new(std::nothrow) Camera(window, glm::radians(90.0f), 0.1f, 1000.0f);
				if (!_camera) { PEX_FATAL("Failed to allocate TerrainGenerationTest's Camera"); }
				if (_camera->getErrorFlag()) { PEX_FATAL("Failed to create TerrainGenerationTest's Camera"); }
				_camera->lockCursor();
			}

			void TerrainGenerationTest::onStop()
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
				PxScene& scene = getScene().getPhysicsScene();
				const glm::vec3& camPos = _camera->getPosition();
				const glm::vec3& camDir = _camera->getForward();
				PxVec3 rayStart(camPos.x, camPos.y, camPos.z);
				PxVec3 rayDir(camDir.x, camDir.y, camDir.z);
				PxRaycastBuffer rayRes;
				scene.lockRead();
				bool hit = scene.raycast(rayStart, rayDir, 1000, rayRes);
				scene.unlockRead();
				if (hit && rayRes.hasBlock) {
					const glm::vec3 hitPos(rayRes.block.position.x, rayRes.block.position.y, rayRes.block.position.z);
					if (_placeAction->getValue()) {
						const glm::i64vec3 chunkSpace(TerrainChunk::WorldToChunkSpace(hitPos - camDir * TerrainChunk::HALF_CHUNK_CELL_SIZE));
						TerrainChunk* terrainChunk = _terrainManager->grabTerrainChunk(TerrainChunk::ChunkSpaceToChunkPosition(chunkSpace));
						uint32_t pointIndex = TerrainChunk::RelativeChunkSpaceToPointIndex(TerrainChunk::ChunkSpaceToRelativeChunkSpace(chunkSpace));
						if (terrainChunk->getPoints()[pointIndex] != 2) {
							terrainChunk->getPoints()[pointIndex] = 2;
							terrainChunk->updateLastModified();
						}

						terrainChunk->drop();
					}
					else if (_breakAction->getValue()) {
						const glm::i64vec3 chunkSpace(TerrainChunk::WorldToChunkSpace(hitPos + camDir * TerrainChunk::HALF_CHUNK_CELL_SIZE));
						TerrainChunk* terrainChunk = _terrainManager->grabTerrainChunk(TerrainChunk::ChunkSpaceToChunkPosition(chunkSpace));
						uint32_t pointIndex = TerrainChunk::RelativeChunkSpaceToPointIndex(TerrainChunk::ChunkSpaceToRelativeChunkSpace(chunkSpace));
						if (terrainChunk->getPoints()[pointIndex] != 0) {
							terrainChunk->getPoints()[pointIndex] = 0;
							terrainChunk->updateLastModified();
						}

						terrainChunk->drop();
					}

					//glm::vec3 pos(hitPos);
					//PEX_INFO(("Ray Pos: x:" + std::to_string(pos.x) + ", y: " + std::to_string(pos.y) + ", z: " + std::to_string(pos.z)).c_str());
					//glm::i64vec3 chunkSpacePos = TerrainChunk::WorldToChunkSpace(pos);
					//pos = chunkSpacePos;
					//PEX_INFO(("Chunk Space Pos: x:" + std::to_string(pos.x) + ", y: " + std::to_string(pos.y) + ", z: " + std::to_string(pos.z)).c_str());
					//pos = TerrainChunk::ChunkSpaceToChunkPosition(chunkSpacePos);
					//PEX_INFO(("Chunk Pos: x:" + std::to_string(pos.x) + ", y: " + std::to_string(pos.y) + ", z: " + std::to_string(pos.z)).c_str());
					//pos = TerrainChunk::ChunkSpaceToRelativeChunkSpace(chunkSpacePos);
					//PEX_INFO(("Rel Chunk Pos: x:" + std::to_string(pos.x) + ", y: " + std::to_string(pos.y) + ", z: " + std::to_string(pos.z)).c_str());
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

				constexpr uint8_t MAX_REFINE_COUNT = 5;
				uint8_t refineCount = 0;
				TerrainObject* refineQueue[MAX_REFINE_COUNT];

				{
					std::shared_lock lock(_terrainMutex);
					for (auto pair : _terrainObjects) {
						TerrainObject* obj = pair.second;
						if (obj->getMeshBuildPending()) continue;
						if (obj->getMeshOutdated()) {
							obj->setMeshBuildPending();
							obj->grab();
							_threadPool.push_task_priority(&TerrainGenerationTest::jobUpdateTerrain, this, obj);
						}
						else if (obj->getMeshRefinement() == TerrainObject::FAST && obj->getPendingMeshRefinement() != TerrainObject::REFINED && refineCount < MAX_REFINE_COUNT) {
							obj->grab();
							refineQueue[refineCount++] = obj;
						}
					}
				}

				if (refineCount) {
					if (_threadPool.get_tasks_queued() < _threadPool.get_thread_count()) {
						for (uint8_t i = 0; i < refineCount; ++i) {
							refineQueue[i]->setMeshBuildPending();
							_threadPool.push_task(&TerrainGenerationTest::jobUpdateTerrain, this, refineQueue[i]);
						}
					}
					else {
						for (uint8_t i = 0; i < refineCount; ++i) {
							refineQueue[i]->drop();
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

			void TerrainGenerationTest::jobUpdateTerrain(terrain::TerrainObject* terrain) {
				if (!terrain->getTerrainLoaded()) {
					PEX_WARN("Attempted to update unloaded TerrainObject");
					terrain->clearMeshBuildPending();
				}
				else if (terrain->getMeshOutdated()) {
					terrain->buildTerrainMesh(terrain::TerrainObject::FAST);
				}
				else if (terrain->getMeshRefinement() == terrain::TerrainObject::FAST) {
					terrain->buildTerrainMesh(terrain::TerrainObject::REFINED);
				}
				else {
					PEX_WARN("Attempted to update TerrainObject with no updates required");
					terrain->clearMeshBuildPending();
				}

				terrain->drop();
			}

			void TerrainGenerationTest::jobLoadTerrain(const glm::i64vec3& terrainPos)
			{
				using namespace terrain;
				{
					std::shared_lock lock(_terrainMutex);
					auto terrainItr = _terrainObjects.find(terrainPos);
					if (terrainItr != _terrainObjects.end()) return;
				}

				TerrainObject* terrainObject = TerrainObject::create(*_terrainRenderProperties);
				if (!terrainObject) { PEX_FATAL("Failed to create TerrainObject"); }
				terrainObject->loadTerrainChunk(terrainPos, *_terrainManager);
				terrainObject->grab();
				terrainObject->setMeshBuildPending();
				_terrainMutex.lock();
				bool inserted = _terrainObjects.emplace(terrainPos, terrainObject).second;
				_terrainMutex.unlock();

				if (inserted) {
					if (!getScene().addObject(terrainObject->getObject())) {
						PEX_FATAL("Failed to add TerrainObject to TerrainGenerationTest's PxeScene");
					}

					terrainObject->buildTerrainMesh(terrain::TerrainObject::REFINED);
				}
				else {
					PEX_WARN(("Attempted to load terrain that was already loaded, x: " + std::to_string(terrainPos.x) + " y: " + std::to_string(terrainPos.y) + " z: " + std::to_string(terrainPos.z)).c_str());
					terrainObject->unloadTerrainChunk();
					terrainObject->drop();
				}

				terrainObject->drop();
			}

			void TerrainGenerationTest::jobUnloadTerrain(const glm::i64vec3& terrainPos)
			{
				using namespace terrain;
				TerrainObject* terrainObject;
				{
					std::unique_lock lock(_terrainMutex);
					auto terrainItr = _terrainObjects.find(terrainPos);
					if (terrainItr == _terrainObjects.end()) return;
					terrainObject = terrainItr->second;
					_terrainObjects.erase(terrainItr);
				}

				terrainObject->unloadTerrainChunk();
				getScene().removeObject(terrainObject->getObject());
				terrainObject->drop();
			}
		}
	}
}
