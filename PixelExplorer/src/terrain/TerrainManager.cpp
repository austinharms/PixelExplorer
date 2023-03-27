#include "TerrainManager.h"

#include <new>
#include <string>

#include "Log.h"
#include "Application.h"

namespace pixelexplorer {
	namespace terrain {
		TerrainManager::TerrainManager(TerrainGenerator& generator)
		{
			_terrainGenerator = &generator;
			generator.grab();
		}

		TerrainManager::~TerrainManager()
		{
			{
				std::unique_lock lock(_terrainMutex);
				for (auto chunkPair : _loadedTerrain) {
					chunkPair.second->setDropCallback(nullptr);
					const glm::i64vec3& pos = chunkPair.second->getPosition();
					PEX_WARN(("Chunk at x:" + std::to_string(pos.x) + ", y:" + std::to_string(pos.y) + ", z:" + std::to_string(pos.z) + " not unloaded").c_str());
				}

				_loadedTerrain.clear();
			}

			_terrainGenerator->drop();
			_terrainGenerator = nullptr;
		}

		TerrainGenerator& TerrainManager::getTerrainGenerator()
		{
			return *_terrainGenerator;
		}

		TerrainChunk* TerrainManager::grabTerrainChunk(const glm::i64vec3& position)
		{
			TerrainChunk* chunk = grabLoadedTerrainChunk(position);
			if (!chunk) {
				{
					std::unique_lock lock(_terrainMutex);
					chunk = new(std::nothrow) TerrainChunk(position);
					if (!chunk) {
						PEX_FATAL("Failed to allocate Terrain Chunk");
						return nullptr;
					}

					_loadedTerrain.emplace(position, chunk);
				}

				chunk->setDropCallback(this);
				_terrainGenerator->generateTerrainData(*chunk);
			}

			return chunk;
		}

		bool TerrainManager::getTerrainChunkLoaded(const glm::i64vec3& position)
		{
			std::shared_lock lock(_terrainMutex);
			return _loadedTerrain.find(position) != _loadedTerrain.end();
		}

		TerrainChunk* TerrainManager::grabLoadedTerrainChunk(const glm::i64vec3& position)
		{
			std::shared_lock lock(_terrainMutex);
			auto chunkItr = _loadedTerrain.find(position);
			if (chunkItr != _loadedTerrain.end()) {
				chunkItr->second->grab();
				return chunkItr->second;
			}

			return nullptr;
		}

		void TerrainManager::operator()(TerrainChunk& chunk)
		{
			std::unique_lock lock(_terrainMutex);
			_loadedTerrain.erase(chunk.getPosition());
		}
	}
}
