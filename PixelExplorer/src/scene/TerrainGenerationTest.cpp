#include "TerrainGenerationTest.h"

#include <new>

#include "Application.h"
#include "terrain/TerrainGenerator.h"
#include "terrain/FlatTerrainGenerator.h"
#include "PxeEngine.h"
#include "PxeRenderMaterial.h"
#include "PxeFSHelpers.h"
#include "Log.h"

namespace pixelexplorer {
	namespace scene {
		TerrainGenerationTest::TerrainGenerationTest()
		{
			using namespace terrain;
			_terrainManager = nullptr;
			_mesh = nullptr;

			TerrainGenerator* gen = new(std::nothrow) FlatTerrainGenerator();
			if (!gen) {
				Application::Error("Failed to create Terrain Generator");
				return;
			}

			_terrainManager = new(std::nothrow) TerrainManager(*gen);
			if (!_terrainManager) {
				gen->drop();
				Application::Error("Failed to create Terrain Manager");
				return;
			}

			gen->drop();
		}

		TerrainGenerationTest::~TerrainGenerationTest()
		{
			if (_mesh) {
				getScene()->removeRenderable(*_mesh);
				_mesh->unloadChunks();
				_mesh->drop();
			}

			delete _terrainManager;
		}

		void TerrainGenerationTest::update()
		{
			if (!_mesh) {
				using namespace pxengine;
				PxeShader* shader = pxeGetEngine().loadShader(getAssetPath("shaders") / "terrain.pxeshader");
				if (!shader) {
					PEX_ERROR("Failed to load terrain shader");
					return;
				}

				PxeRenderMaterial* material = new(std::nothrow) PxeRenderMaterial(*shader);
				if (!material) {
					shader->drop();
					PEX_ERROR("Failed to create terrain material");
					return;
				}

				_mesh = new(std::nothrow) terrain::TerrainRenderMesh(*material);
				if (!_mesh) {
					shader->drop();
					material->drop();
					PEX_ERROR("Failed to create terrain mesh");
					return;
				}

				_mesh->loadChunks(glm::i64vec3(0, 0, 0), *_terrainManager);
				_mesh->rebuildMesh();
				material->drop();
				shader->drop();

				getScene()->addRenderable(*_mesh);
			}
			else {
				_mesh->rebuildMesh();
			}
			//_terrainManager->getTerrainChunk(glm::i64vec3(0, 0, _pos++))->grab();
		}
	}
}
