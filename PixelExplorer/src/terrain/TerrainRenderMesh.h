#ifndef PIXELEXPLORER_TERRAIN_TERRAIN_RENDERE_MESH_H_
#define PIXELEXPLORER_TERRAIN_TERRAIN_RENDERE_MESH_H_
#include "PxeStaticPhysicsRenderObject.h"
#include "glm/vec3.hpp"
#include "TerrainChunk.h"
#include "PxeRenderMaterial.h"
#include "TerrainManager.h"
#include "PxeVertexArray.h"
#include "PxeIndexBuffer.h"
#include "PxeVertexBuffer.h"
#include "PxeBuffer.h"

namespace pixelexplorer {
	namespace terrain {
		class TerrainRenderMesh : public pxengine::PxeStaticPhysicsRenderObject
		{
		public:
			TerrainRenderMesh(pxengine::PxeRenderMaterial& chunkMaterial);
			virtual ~TerrainRenderMesh();
			void loadChunks(const glm::i64vec3& chunkPos, TerrainManager& mgr);
			void unloadChunks();
			bool meshRebuildRequired() const;
			uint64_t getLastMeshUpdate() const;
			void rebuildMesh();
			void onRender() override;

		private:
			enum Chunk
			{
				CENTER = 0,
				RIGHT,
				FORWARD,
				RIGHT_FORWARD,
				TOP_CENTER,
				TOP_RIGHT,
				TOP_FORWARD,
				TOP_RIGHT_FORWARD,
				CHUNK_COUNT
			};

			TerrainChunk::ChunkPoint getRelativeChunkPoint(uint32_t x, uint32_t y, uint32_t z);

			uint64_t _lastGenerated;
			TerrainChunk* _chunks[CHUNK_COUNT];
			pxengine::PxeIndexBuffer* _meshIndexBuffer;
			pxengine::PxeVertexArray* _meshVertexArray;
			pxengine::PxeVertexBuffer* _meshVertexBuffer;
		};
	}
}
#endif // !PIXELEXPLORER_TERRAIN_TERRAIN_RENDERE_MESH_H_
