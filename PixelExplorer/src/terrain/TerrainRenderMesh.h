#ifndef PIXELEXPLORER_TERRAIN_TERRAIN_RENDERE_MESH_H_
#define PIXELEXPLORER_TERRAIN_TERRAIN_RENDERE_MESH_H_
#include <vector>
#include <mutex>

#include "PxeStaticPhysicsRenderObject.h"
#include "glm/vec3.hpp"
#include "TerrainChunk.h"
#include "PxeRenderMaterialInterface.h"
#include "TerrainManager.h"
#include "PxeVertexArray.h"
#include "PxeIndexBuffer.h"
#include "PxeVertexBuffer.h"
#include "PxeBuffer.h"
#include "PxShape.h"

namespace pixelexplorer {
	namespace terrain {
		class TerrainRenderMesh : public pxengine::PxeStaticPhysicsRenderObject
		{
		public:
			enum MeshType : uint8_t {
				NONE = 0,
				FAST,
				REFINED
			};

			TerrainRenderMesh(pxengine::PxeRenderMaterialInterface& chunkMaterial);
			virtual ~TerrainRenderMesh();
			void loadChunks(const glm::i64vec3& chunkPos, TerrainManager& mgr);
			void unloadChunks();
			bool getMeshOutdated() const;
			uint64_t getCurrentMeshTimestamp() const;
			uint64_t getWorkingMeshTimestamp() const;
			MeshType getCurrentMeshType() const;
			MeshType getWorkingMeshType() const;
			bool getWorkPending() const;
			void setWorkPending();
			void resetWorkPending();
			void rebuildMesh(MeshType type = REFINED);
			void onRender() override;
			glm::i64vec3 getChunkPosition() const;

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

			TerrainChunk::ChunkPoint getRelativeChunkPoint(uint32_t x, uint32_t y, uint32_t z) const;
			physx::PxShape* buildPhysicsShape(const std::vector<uint16_t>& indices, const std::vector<glm::vec3>& vertices) const;
			void buildTerrainMesh(std::vector<uint16_t>& indices, std::vector<glm::vec3>& vertices) const;
			void buildTerrainMeshFast(std::vector<uint16_t>& indices, std::vector<glm::vec3>& vertices) const;
			void updateRenderMesh(const std::vector<uint16_t>& indices, const std::vector<glm::vec3>& vertices);

			mutable std::mutex _meshWorkMutex;
			uint64_t _currentMeshDate;
			uint64_t _latestWorkingMeshDate;
			MeshType _latestWorkingMeshType;
			MeshType _currentMeshType;
			bool _workPending;
			bool _hasMesh;
			
			TerrainChunk* _chunks[CHUNK_COUNT];
			pxengine::PxeIndexBuffer* _meshIndexBuffer;
			pxengine::PxeVertexArray* _meshVertexArray;
			pxengine::PxeVertexBuffer* _meshVertexBuffer;
		};
	}
}
#endif // !PIXELEXPLORER_TERRAIN_TERRAIN_RENDERE_MESH_H_
