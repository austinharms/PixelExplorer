#include "TerrainGenerationTest.h"

#include <new>

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

			for (int64_t x = -10; x < 10; ++x) {
				for (int64_t y = -10; y < 10; ++y) {
					for (int64_t z = -10; z < 10; ++z) {
						glm::i64vec3 pos(x, y, z);
						terrain::TerrainRenderMesh* terrainMesh = new(std::nothrow) terrain::TerrainRenderMesh(*_terrainRenderMaterial);
						if (!terrainMesh) {
							Application::Error("Out of Memory, Failed to create Terrain Render Mesh");
							return;
						}

						getScene()->addObject(*terrainMesh);
						terrainMesh->loadChunks(pos, *_terrainManager);
						terrainMesh->rebuildMesh();
						_terrainChunks.emplace(pos, terrainMesh);
					}
				}
			}

			//glm::i64vec3 pos(0, 0, 0);
			//terrain::TerrainRenderMesh* terrainMesh = new(std::nothrow) terrain::TerrainRenderMesh(*_terrainRenderMaterial);
			//if (!terrainMesh) {
			//	Application::Error("Out of Memory, Failed to create Terrain Render Mesh");
			//	return;
			//}

			//getScene()->addRenderable(*terrainMesh);
			//terrainMesh->loadChunks(pos, *_terrainManager);
			//terrainMesh->rebuildMesh();
			//_terrainChunks.emplace(pos, terrainMesh);
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
