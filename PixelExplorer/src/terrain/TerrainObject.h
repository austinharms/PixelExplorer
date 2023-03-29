#ifndef PIXELEXPLORER_TERRAIN_OBJECT_H_
#define PIXELEXPLORER_TERRAIN_OBJECT_H_
#include <mutex>

#include "PxeTypes.h"
#include "PxeRefCount.h"
#include "PxeObject.h"
#include "rendering/TerrainRenderProperties.h"
#include "rendering/TerrainRenderComponent.h"
#include "PxeStaticPhysicsActor.h"
#include "TerrainManager.h"
#include "TerrainChunk.h"

namespace pixelexplorer {
	namespace terrain {
		class TerrainObject : public pxengine::PxeRefCount
		{
		public:
			enum MeshRefinement : uint8_t {
				NO_MESH = 0,
				FAST,
				REFINED
			};

			static TerrainObject* create(TerrainRenderProperties& renderProperties);
			PXE_NODISCARD pxengine::PxeObject& getObject();
			PXE_NODISCARD const glm::i64vec3& getLoaddedTerrainPosition() const;
			PXE_NODISCARD bool getTerrainLoaded() const;
			PXE_NODISCARD bool getMeshBuildPending() const;
			PXE_NODISCARD uint64_t getMeshTimestamp() const;
			PXE_NODISCARD MeshRefinement getMeshRefinement() const;
			PXE_NODISCARD uint64_t getPendingMeshTimestamp() const;
			PXE_NODISCARD MeshRefinement getPendingMeshRefinement() const;
			PXE_NODISCARD bool getMeshOutdated() const;
			void buildTerrainMesh(MeshRefinement refinement = REFINED);
			void loadTerrainChunk(const glm::i64vec3& chunkPos, TerrainManager& terrainMgr);
			void unloadTerrainChunk();
			void clearMeshBuildPending();
			void setMeshBuildPending();
			virtual ~TerrainObject();
			PXE_NOCOPY(TerrainObject);

		protected:
			TerrainObject(pxengine::PxeObject& object, TerrainRenderComponent& renderComponent, pxengine::PxeStaticPhysicsActor& physicsActor);

		private:
			enum RelativeChunkPos
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

			struct MeshBuildData;

			TerrainChunk::ChunkPoint getRelativeChunkPoint(uint32_t x, uint32_t y, uint32_t z) const;
			void buildRenderMeshFast(MeshBuildData& data);
			void buildPhysicsMeshFast(MeshBuildData& data);
			//void buildRenderMeshRefined(MeshBuildData& data);
			//void buildPhysicsMeshRefined(MeshBuildData& data);

			mutable std::mutex _workMutex;
			uint64_t _currentTimestamp;
			uint64_t _pendingTimestamp;
			pxengine::PxeObject& _object;
			TerrainRenderComponent& _renderComponent;
			pxengine::PxeStaticPhysicsActor& _physicsActor;
			pxengine::PxePhysicsShape* _physicsShape;
			MeshRefinement _currentRefinement;
			MeshRefinement _pendingRefinement;
			TerrainChunk* _neighboringChunks[CHUNK_COUNT];
			glm::i64vec3 _terrainPosition;
			bool _meshBuildPending;
			bool _terrainLoaded;
		};
	}
}
#endif // !PIXELEXPLORER_TERRAIN_OBJECT_H_
