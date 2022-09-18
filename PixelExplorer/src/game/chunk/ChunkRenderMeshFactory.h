#include <queue>
#include <mutex>

#include "common/RefCount.h"
#include "ChunkRenderMesh.h"
#include "engine/rendering/RenderWindow.h"
#include "engine/rendering/Material.h"

#ifndef PIXELEXPLORER_GAME_CHUNK_CHUNKRENDERMESHFACTORY_H_
#define PIXELEXPLORER_GAME_CHUNK_CHUNKRENDERMESHFACTORY_H_
namespace pixelexplorer::game::chunk {
	class ChunkRenderMeshFactory : public RefCount
	{
	public:
		// a class used to create and reuse ChunkRenderMesh's
		ChunkRenderMeshFactory(engine::rendering::RenderWindow& window);
		virtual ~ChunkRenderMeshFactory();

		// returns a ChunkRenderMesh object, new or used
		// note you do not need to grab the returned object
		ChunkRenderMesh* getRenderMesh();

		// insert a ChunkRenderMesh into the queue to be reused
		void addRenderMesh(ChunkRenderMesh& mesh);

		// returns the material used in all ChunkRenderMesh's
		engine::rendering::Material& getMaterial() const;

		// empties the internal ChunkRenderMesh queue 
		void emptyRenderMeshQueue();

	private:
		engine::rendering::RenderWindow& _window;
		engine::rendering::Material* _chunkMaterial;
		std::queue<ChunkRenderMesh*> _renderMeshQueue;
		std::mutex _renderMeshQueueMutex;
	};
}
#endif 