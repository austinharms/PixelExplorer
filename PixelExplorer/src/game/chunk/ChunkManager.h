#include <unordered_map>
#include <mutex>

#include "common/RefCount.h"
#include "Chunk.h"
#include "ChunkRenderMeshFactory.h"
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
		// TODO add description here
		ChunkManager(ChunkRenderMeshFactory& chunkRenderMeshFactory, block::BlockManifest& blockManifest);
		virtual ~ChunkManager();

		// returns the chunk at pos if the chunk is loaded
		// if forceLoad is true it will load the chunk if it is not already loaded
		// returns null if the chunk was unloaded or failed to load
		Chunk* getChunk(const glm::i32vec3& pos, bool forceLoad = false);

		// removes the chunk at pos from being tracked by this ChunkManager and removes the chunk's ChunkRenderMesh
		// logs a warning if the chunk was not loaded
		void unloadChunk(const glm::i32vec3& pos);

		// calls unloadChunk on all of the loaded chunks tracked by this ChunkManager
		void unloadAllChunks();

	private:
		block::BlockManifest& _blockManifest;
		ChunkRenderMeshFactory& _renderMeshFactory;
		std::unordered_map<glm::i32vec3, Chunk*> _loadedChunks;
		std::recursive_mutex _chunkMutex;

		// loads or generates a chunk at pos
		// note this method dose not check if that chunk was already loaded
		// you should almost always use getChunk instead (getChunk calls this internally)
		Chunk* loadChunk(const glm::i32vec3& pos);

		// rebuilds the chunk's ChunkRenderMesh mesh
		void remeshChunk(Chunk& chunk);

		// removes the chunk's ChunkRenderMesh and drops the chunk
		void unloadChunk(Chunk& chunk);
	};
}
#endif // !PIXELEXPLORER_GAME_CHUNK_CHUNKMANAGER_H_
