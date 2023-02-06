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
		TerrainGenerationTest::TerrainGenerationTest()
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
			if (_pauseMenu)
				_pauseMenu->drop();

			if (_pauseAction)
				_pauseAction->drop();

			for (auto meshPair : _terrainChunks) {
				if (meshPair.second) {
					meshPair.second->unloadChunks();
					meshPair.second->drop();
				}
			}

			_terrainChunks.clear();

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
				using namespace terrain;
				uint64_t primaryTime = SDL_GetTicks64();
				glm::i64vec3 cameraChunkPos = TerrainChunk::ChunkSpaceToChunkPosition(TerrainChunk::WorldToChunkSpace(_camera->getPosition()));
				for (int64_t x = -1; x < 2; ++x) {
					for (int64_t y = -1; y < 2; ++y) {
						for (int64_t z = -1; z < 2; ++z) {
							glm::i64vec3 pos = glm::i64vec3(x, y, z) + cameraChunkPos;
							auto terrainItr = _terrainChunks.find(pos);
							if (terrainItr == _terrainChunks.end()) {
								TerrainRenderMesh* terrainMesh = new(std::nothrow) TerrainRenderMesh(*_terrainRenderMaterial);
								if (!terrainMesh) {
									Application::Error("Out of Memory, Failed to create Terrain Render Mesh");
									return;
								}

								getScene()->addObject(*terrainMesh);
								terrainMesh->loadChunks(pos, *_terrainManager);
								terrainMesh->rebuildMesh();
								_terrainChunks.emplace(pos, terrainMesh);
							}
							else {
								TerrainRenderMesh* terrainMesh = terrainItr->second;
								if (terrainMesh->meshRebuildRequired())
									terrainMesh->rebuildMesh();
							}
						}
					}
				}

				PEX_INFO(("Required chunk loading, took: " + std::to_string(SDL_GetTicks64() - primaryTime) + "ms").c_str());

				if (llabs(cameraChunkPos.x - _lastLoadedChunkPosition.x) >= 2 || 
					llabs(cameraChunkPos.y - _lastLoadedChunkPosition.y) >= 2 || 
					llabs(cameraChunkPos.z - _lastLoadedChunkPosition.z) >= 2) 
				{
					uint64_t startTime = SDL_GetTicks64();
					static constexpr int64_t loadDistance = 5;
					for (int64_t x = -loadDistance; x < loadDistance + 1; ++x) {
						for (int64_t y = -loadDistance; y < loadDistance + 1; ++y) {
							for (int64_t z = -loadDistance; z < loadDistance + 1; ++z) {
								glm::i64vec3 loopPos(x, y, z);
								glm::i64vec3 unloadPos = loopPos + _lastLoadedChunkPosition;
								glm::i64vec3 loadPos = loopPos + cameraChunkPos;

								// Check if unload position is out of loaded area
								if (llabs(unloadPos.x - cameraChunkPos.x) > loadDistance ||
									llabs(unloadPos.y - cameraChunkPos.y) > loadDistance ||
									llabs(unloadPos.z - cameraChunkPos.z) > loadDistance)
								{
									auto terrainItr = _terrainChunks.find(unloadPos);
									if (terrainItr != _terrainChunks.end()) {
										TerrainRenderMesh* mesh = terrainItr->second;
										_terrainChunks.erase(terrainItr);
										getScene()->removeObject(*mesh);
										mesh->unloadChunks();
										mesh->drop();
									}
								}

								// Check if load position was already loaded
								if (llabs(loadPos.x - _lastLoadedChunkPosition.x) > loadDistance ||
									llabs(loadPos.y - _lastLoadedChunkPosition.y) > loadDistance ||
									llabs(loadPos.z - _lastLoadedChunkPosition.z) > loadDistance) 
								{
									TerrainRenderMesh* terrainMesh = new(std::nothrow) TerrainRenderMesh(*_terrainRenderMaterial);
									if (!terrainMesh) {
										Application::Error("Out of Memory, Failed to create Terrain Render Mesh");
										return;
									}

									bool inserted = _terrainChunks.emplace(loadPos, terrainMesh).second;
									if (inserted) {
										getScene()->addObject(*terrainMesh);
										terrainMesh->loadChunks(loadPos, *_terrainManager);
										terrainMesh->rebuildMesh();
									}
									else {
										PEX_WARN(("Attempted to load terrain that was already loaded, x: " + std::to_string(loadPos.x) + " y: " + std::to_string(loadPos.y) + " z: " + std::to_string(loadPos.z)).c_str());
										terrainMesh->drop();
									}
								}
							}
						}
					}

					_lastLoadedChunkPosition = cameraChunkPos;
					PEX_INFO(("Updated chunk loading, took: " + std::to_string(SDL_GetTicks64() - startTime) + "ms").c_str());
				}

				for (auto pair : _terrainChunks) {
					if (pair.second->meshRebuildRequired())
						pair.second->rebuildMesh();
				}
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
	}
}
