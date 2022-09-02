#include <stdint.h>
#include <unordered_map>
#include <queue>
#include <mutex>

#include "Logger.h"
#include "RefCount.h"
#include "rendering/Material.h"
#include "rendering/Shader.h"
#include "rendering/RenderWindow.h"
#include "ChunkRenderMesh.h"
#include "Chunk.h"
#include "../block/BlockManifest.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/vec3.hpp"
#include "glm/gtx/hash.hpp"


#ifndef PIXELEXPLORER_GAME_CHUNK_CHUNKMANAGER_H_
#define PIXELEXPLORER_GAME_CHUNK_CHUNKMANAGER_H_
namespace pixelexplorer::game::chunk {
	class ChunkManager : public RefCount
	{
	public:
		inline ChunkManager(rendering::RenderWindow* window, block::BlockManifest* blockManifest) {
			window->grab();
			_renderWindow = window;
			blockManifest->grab();
			_blockManifest = blockManifest;
			_chunkMaterial = new rendering::Material();
			_chunkMaterial->setProperty("u_Color", glm::vec4(0, 1, 0, 1));
			_renderWindow->registerGLObject(_chunkMaterial);
			rendering::Shader* chunkShader = _renderWindow->getShader("./assets/shaders/cube.shader");
			_chunkMaterial->addDependency(chunkShader);
			chunkShader->drop();
			chunkShader = nullptr;
		}

		inline virtual ~ChunkManager() {
			_chunkMutex.lock();
			unloadAllChunks();
			dropUnusedRenderMeshes();
			_chunkMutex.unlock();
			_chunkMaterial->drop();
			_renderWindow->drop();
			_blockManifest->drop();
		}

		inline Chunk* getChunk(const glm::i32vec3& pos, bool forceLoad = false) {
			std::lock_guard<std::recursive_mutex> lock(_chunkMutex);
			auto chunk = _loadedChunks.find(pos);
			if (chunk != _loadedChunks.end())
				return chunk->second;
			if (forceLoad)
				return loadChunk(pos);
			return nullptr;
		}

		inline Chunk* loadChunk(const glm::i32vec3& pos) {
			ChunkRenderMesh* renderMesh;
			_chunkMutex.lock();
			if (_unusedRenderMeshes.empty()) {
				renderMesh = new ChunkRenderMesh(_chunkMaterial, _renderWindow);
			}
			else {
				renderMesh = _unusedRenderMeshes.front();
				_unusedRenderMeshes.pop();
			}

			Chunk* chunk = new Chunk(renderMesh, pos);
			_loadedChunks.insert({ glm::i32vec3(pos), chunk });
			_chunkMutex.unlock();
			remeshChunk(chunk);
			_chunkMaterial->getRenderWindow()->addGLRenderObject(renderMesh);
			renderMesh->drop();
			return chunk;
		}

		inline void unloadChunk(const glm::i32vec3& pos) {
			_chunkMutex.lock();
			auto chunkIterator = _loadedChunks.find(pos);
			if (chunkIterator == _loadedChunks.end()) {
				_chunkMutex.unlock();
				Logger::warn(__FUNCTION__" attempted to unload unloaded chunk");
				return;
			}

			Chunk* chunk = chunkIterator->second;
			_loadedChunks.erase(chunkIterator);
			ChunkRenderMesh* renderMesh = chunk->getRenderMesh();
			renderMesh->grab();
			_renderWindow->removeGLRenderObject(renderMesh);
			_unusedRenderMeshes.emplace(renderMesh);
			_chunkMutex.unlock();
			if (!chunk->drop())
				Logger::warn(__FUNCTION__" loaded chunk not dropped");
			chunk = nullptr;
		}

		inline void unloadAllChunks() {
			_chunkMutex.lock();
			auto it = _loadedChunks.begin();
			while (it != _loadedChunks.end()) {
				const glm::i32vec3 chunkPos = it->first;
				it = ++it;
				unloadChunk(chunkPos);
			}

			_chunkMutex.unlock();
		}

		inline void dropUnusedRenderMeshes() {
			_chunkMutex.lock();
			while (!_unusedRenderMeshes.empty()) {
				_unusedRenderMeshes.front()->drop();
				_unusedRenderMeshes.pop();
			}

			_chunkMutex.unlock();
		}

		inline void remeshChunk(Chunk* chunk) {
			if (chunk == nullptr) {
				Logger::warn(__FUNCTION__" attempted to remesh null chunk");
				return;
			}

			chunk->grab();
			ChunkRenderMesh* renderMesh = chunk->getRenderMesh();
			if (renderMesh == nullptr) {
				Logger::warn(__FUNCTION__" attempted to remesh chunk with null render mesh");
				chunk->drop();
				return;
			}

			renderMesh->grab();
			// remesh chunk
			renderMesh->drop();
			chunk->drop();
		}

	private:
		rendering::RenderWindow* _renderWindow;
		block::BlockManifest* _blockManifest;
		rendering::Material* _chunkMaterial;
		std::unordered_map<glm::i32vec3, Chunk*> _loadedChunks;
		std::queue<ChunkRenderMesh*> _unusedRenderMeshes;
		std::recursive_mutex _chunkMutex;
	};
}
#endif // !PIXELEXPLORER_GAME_CHUNK_CHUNKMANAGER_H_
