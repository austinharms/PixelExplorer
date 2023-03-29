#include "TerrainObject.h"

#include "PxeObject.h"
#include "PxeVectorBuffer.h"
#include "Log.h"
#include "SDL_timer.h"
#include "rendering/MeshGenerationTables.h"
#include "cooking/PxTriangleMeshDesc.h"
#include "PxMaterial.h"
#include "PxeEngine.h"

const glm::vec3 TerrainColorTable[4] = { glm::vec3(1, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1) };

namespace pixelexplorer {
	namespace terrain {
		struct TerrainObject::MeshBuildData {
			MeshBuildData(pxengine::PxeVectorBuffer<uint16_t>& indexBuf, pxengine::PxeVectorBuffer<glm::vec3>& vertexBuf) :
				indexBuffer(indexBuf), vertexBuffer(vertexBuf) {
				shape = nullptr;
			}

			pxengine::PxeVectorBuffer<uint16_t>& indexBuffer;
			pxengine::PxeVectorBuffer<glm::vec3>& vertexBuffer;
			physx::PxShape* shape;
		};

		TerrainObject* TerrainObject::create(TerrainRenderProperties& renderProperties)
		{
			using namespace pxengine;
			PxeObject* obj = PxeObject::create();
			if (!obj) {
				PEX_ERROR("Failed to create TerrainObject's PxeObject");
				return nullptr;
			}

			TerrainRenderComponent* renderComponent = TerrainRenderComponent::create(renderProperties);
			if (!renderComponent) {
				PEX_ERROR("Failed to create TerrainObject's TerrainRenderComponent");
				obj->drop();
				return nullptr;
			}

			PxeStaticPhysicsActor* actor = PxeStaticPhysicsActor::create();
			if (!actor) {
				PEX_ERROR("Failed to create TerrainObject's PxeStaticPhysicsActor");
				obj->drop();
				renderComponent->drop();
				return nullptr;
			}

			if (!obj->addComponent(*renderComponent)) {
				PEX_ERROR("Failed to add TerrainRenderComponent to TerrainObject's PxeObject");
				obj->drop();
				renderComponent->drop();
				actor->drop();
				return nullptr;
			}

			if (!obj->addComponent(*actor)) {
				PEX_ERROR("Failed to add PxeStaticPhysicsActor to TerrainObject's PxeObject");
				obj->drop();
				renderComponent->drop();
				actor->drop();
				return nullptr;
			}

			TerrainObject* terrainObj = new(std::nothrow) TerrainObject(*obj, *renderComponent, *actor);
			if (!terrainObj) {
				PEX_ERROR("Failed to allocate TerrainObject");
				obj->drop();
				renderComponent->drop();
				actor->drop();
				return nullptr;
			}

			return terrainObj;
		}

		TerrainObject::TerrainObject(pxengine::PxeObject& object, TerrainRenderComponent& renderComponent, pxengine::PxeStaticPhysicsActor& physicsActor) :
			_object(object), _renderComponent(renderComponent), _physicsActor(physicsActor)
		{
			_physicsShape = nullptr;
			_currentTimestamp = 0;
			_pendingTimestamp = 0;
			_currentRefinement = NO_MESH;
			_pendingRefinement = NO_MESH;
			_meshBuildPending = false;
			_terrainLoaded = false;
			_terrainPosition = glm::i64vec3(0);
		}

		TerrainObject::~TerrainObject()
		{
			if (_terrainLoaded)
				unloadTerrainChunk();
			_renderComponent.drop();
			_physicsActor.drop();
			_object.drop();
			if (_physicsShape) _physicsShape->drop();
		}

		PXE_NODISCARD pxengine::PxeObject& TerrainObject::getObject()
		{
			return _object;
		}

		PXE_NODISCARD const glm::i64vec3& TerrainObject::getLoaddedTerrainPosition() const
		{
			return _terrainPosition;
		}

		PXE_NODISCARD bool TerrainObject::getTerrainLoaded() const
		{
			return _terrainLoaded;
		}

		PXE_NODISCARD bool TerrainObject::getMeshBuildPending() const
		{
			return _meshBuildPending;
		}

		PXE_NODISCARD uint64_t TerrainObject::getMeshTimestamp() const
		{
			return _currentTimestamp;
		}

		PXE_NODISCARD TerrainObject::MeshRefinement TerrainObject::getMeshRefinement() const
		{
			return _currentRefinement;
		}

		PXE_NODISCARD uint64_t TerrainObject::getPendingMeshTimestamp() const
		{
			return _pendingTimestamp;
		}

		PXE_NODISCARD TerrainObject::MeshRefinement TerrainObject::getPendingMeshRefinement() const
		{
			return _pendingRefinement;
		}

		PXE_NODISCARD bool TerrainObject::getMeshOutdated() const
		{
			std::lock_guard lock(_workMutex);
			for (uint8_t i = 0; i < CHUNK_COUNT; ++i)
			{
				if (_neighboringChunks[i] && _neighboringChunks[i]->getLastModified() >= _pendingTimestamp)
					return true;
			}

			return false;
		}

		void TerrainObject::buildTerrainMesh(MeshRefinement refinement)
		{
			_workMutex.lock();
			uint64_t curTimestamp = SDL_GetTicks64();
			_pendingTimestamp = curTimestamp;
			_pendingRefinement = refinement;
			_workMutex.unlock();
			clearMeshBuildPending();

			pxengine::PxeVectorBuffer<uint16_t>* indexBuf = pxengine::PxeVectorBuffer<uint16_t>::create<uint16_t>();
			if (!indexBuf) { PEX_FATAL("Failed to create MeshBuildData's index PxeVectorBuffer"); }
			pxengine::PxeVectorBuffer<glm::vec3>* vertBuf = pxengine::PxeVectorBuffer<glm::vec3>::create<glm::vec3>();
			if (!vertBuf) { PEX_FATAL("Failed to create MeshBuildData's vertex PxeVectorBuffer"); }
			MeshBuildData buildData(*indexBuf, *vertBuf);
			buildRenderMeshFast(buildData);
			buildPhysicsMeshFast(buildData);

			std::lock_guard lock(_workMutex);
			if (_currentTimestamp > curTimestamp || _currentTimestamp == curTimestamp && _currentRefinement > refinement) {
				if (buildData.shape) buildData.shape->release();
				indexBuf->drop();
				vertBuf->drop();
				return;
			}

			if (_physicsShape)
				_object.removeComponent(*_physicsShape);
			_physicsShape = nullptr;

			if (buildData.shape) {
				_physicsShape = new(std::nothrow) pxengine::PxePhysicsShape(*buildData.shape);
				if (!_physicsShape) {
					buildData.shape->release();
					PEX_FATAL("Failed to allocate TerrainObject's PxePhysicsShape");
				}

				if (!_object.addComponent(*_physicsShape)) {
					PEX_ERROR("Failed to add TerrainObject's PxePhysicsShape to TerrainObject's PxeObject");
				}
			}

			if (buildData.indexBuffer.getSize() && buildData.vertexBuffer.getSize()) {
				_renderComponent.clearRenderMesh();
				_renderComponent.setRenderMesh(buildData.indexBuffer, buildData.vertexBuffer);
			}
			else {
				_renderComponent.clearRenderMesh();
			}

			indexBuf->drop();
			vertBuf->drop();
			_currentTimestamp = curTimestamp;
			_currentRefinement = refinement;
		}

		void TerrainObject::loadTerrainChunk(const glm::i64vec3& chunkPos, TerrainManager& terrainMgr)
		{
			std::lock_guard lock(_workMutex);
			if (_terrainLoaded) unloadTerrainChunk();
			_terrainPosition = chunkPos;
			constexpr glm::i64vec3 chunkOffsets[CHUNK_COUNT] = {
				glm::i64vec3(0, 0, 0), // CENTER
				glm::i64vec3(1, 0, 0), // RIGHT
				glm::i64vec3(0, 0, 1), // FORWARD
				glm::i64vec3(1, 0, 1), // RIGHT_FORWARD
				glm::i64vec3(0, 1, 0), // TOP_CENTER
				glm::i64vec3(1, 1, 0), // TOP_RIGHT
				glm::i64vec3(0, 1, 1), // TOP_FORWARD
				glm::i64vec3(1, 1, 1), // TOP_RIGHT_FORWARD
			};

			for (uint8_t i = 0; i < CHUNK_COUNT; ++i) {
				_neighboringChunks[i] = terrainMgr.grabTerrainChunk(chunkPos + chunkOffsets[i]);
			}

			_object.setTransform(glm::translate(glm::scale(glm::mat4(1), glm::vec3(TerrainChunk::CHUNK_CELL_SIZE)), glm::vec3(chunkPos) * (float)TerrainChunk::CHUNK_GRID_SIZE));
			_terrainLoaded = true;
		}

		void TerrainObject::unloadTerrainChunk()
		{
			std::lock_guard lock(_workMutex);
			if (!_terrainLoaded) return;
			_terrainLoaded = false;
			_currentTimestamp = 0;
			_currentRefinement = NO_MESH;
			_pendingTimestamp = 0;
			_pendingRefinement = NO_MESH;
			_meshBuildPending = false;
			_terrainPosition = glm::i64vec3(0);
			if (_physicsShape) {
				_object.removeComponent(*_physicsShape);
				_physicsShape = nullptr;
			}

			_renderComponent.clearRenderMesh();
			for (uint8_t i = 0; i < CHUNK_COUNT; ++i)
			{
				if (_neighboringChunks[i]) {
					_neighboringChunks[i]->drop();
					_neighboringChunks[i] = nullptr;
				}
			}
		}

		void TerrainObject::clearMeshBuildPending()
		{
			_meshBuildPending = false;
		}

		void TerrainObject::setMeshBuildPending()
		{
			_meshBuildPending = true;
		}

		TerrainChunk::ChunkPoint TerrainObject::getRelativeChunkPoint(uint32_t x, uint32_t y, uint32_t z) const {
			glm::u32vec3 localSpace(x, y, z);
			uint8_t chunkIndex = 0;
			if (x >= TerrainChunk::CHUNK_GRID_SIZE) {
				localSpace.x -= TerrainChunk::CHUNK_GRID_SIZE;
				chunkIndex += RIGHT;
			}

			if (y >= TerrainChunk::CHUNK_GRID_SIZE) {
				localSpace.y -= TerrainChunk::CHUNK_GRID_SIZE;
				chunkIndex += TOP_CENTER;
			}

			if (z >= TerrainChunk::CHUNK_GRID_SIZE) {
				localSpace.z -= TerrainChunk::CHUNK_GRID_SIZE;
				chunkIndex += FORWARD;
			}

			TerrainChunk* chunk = _neighboringChunks[chunkIndex];
			if (!chunk) return 0;
			return chunk->getPoints()[localSpace.x + localSpace.z * TerrainChunk::CHUNK_GRID_SIZE + localSpace.y * TerrainChunk::CHUNK_LAYER_POINT_COUNT];
		}

		void TerrainObject::buildRenderMeshFast(MeshBuildData& data)
		{
			std::vector<uint16_t>& indices = data.indexBuffer.getVector<uint16_t>();
			std::vector<glm::vec3>& vertices = data.vertexBuffer.getVector<glm::vec3>();

			for (uint32_t y = 0; y < TerrainChunk::CHUNK_GRID_SIZE; ++y) {
				for (uint32_t z = 0; z < TerrainChunk::CHUNK_GRID_SIZE; ++z) {
					for (uint32_t x = 0; x < TerrainChunk::CHUNK_GRID_SIZE; ++x) {
						uint8_t triIndex = 0;
						TerrainChunk::ChunkPoint points[8] = {
							getRelativeChunkPoint(x, y, z),
							getRelativeChunkPoint(x + 1, y, z),
							getRelativeChunkPoint(x + 1, y, z + 1),
							getRelativeChunkPoint(x, y, z + 1),
							getRelativeChunkPoint(x, y + 1, z),
							getRelativeChunkPoint(x + 1, y + 1, z),
							getRelativeChunkPoint(x + 1, y + 1, z + 1),
							getRelativeChunkPoint(x, y + 1, z + 1)
						};

						if (points[0]) triIndex |= 0x01;
						if (points[1]) triIndex |= 0x02;
						if (points[2]) triIndex |= 0x04;
						if (points[3]) triIndex |= 0x08;
						if (points[4]) triIndex |= 0x10;
						if (points[5]) triIndex |= 0x20;
						if (points[6]) triIndex |= 0x40;
						if (points[7]) triIndex |= 0x80;

						const int8_t* triPoints = data::TriangleTable[triIndex];
						const int8_t* triNormals = data::TriangleNormalTable[triIndex];
						for (uint8_t i = 0; i < 15; ++i) {
							if (triPoints[i] == -1) break;
							glm::vec3 vertex(glm::vec3(x, y, z) + data::TriangleVertices[triPoints[i]]);
							data.indexBuffer.getVector<uint16_t>().emplace_back(vertices.size() / 3);
							vertices.emplace_back(vertex);
							vertices.emplace_back(data::TriangleNormals[triNormals[i / 3]]);
							const uint8_t* colorMix = data::TriangleColorMixingTable[triPoints[i]];
							glm::vec3 color(0);
							if (points[colorMix[0]] && points[colorMix[1]]) {
								color = TerrainColorTable[points[colorMix[0]]] / 2.0f + TerrainColorTable[points[colorMix[1]]] / 2.0f;
							}
							else if (points[colorMix[0]]) {
								color = TerrainColorTable[points[colorMix[0]]];
							}
							else {
								color = TerrainColorTable[points[colorMix[1]]];
							}

							vertices.emplace_back(color);
						}
					}
				}
			}
		}

		void TerrainObject::buildPhysicsMeshFast(MeshBuildData& data)
		{
			using namespace physx;
			std::vector<uint16_t>& indices = data.indexBuffer.getVector<uint16_t>();
			std::vector<glm::vec3>& vertices = data.vertexBuffer.getVector<glm::vec3>();
			pxengine::PxeEngine& engine = pxengine::PxeEngine::getInstance();
			if (vertices.size() == 0 || indices.size() == 0) return;
			PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = vertices.size() / 3;
			meshDesc.points.data = &(vertices[0]);
			meshDesc.points.stride = sizeof(float) * 9;
			meshDesc.triangles.data = &(indices[0]);
			meshDesc.triangles.stride = sizeof(uint16_t) * 3;
			meshDesc.triangles.count = indices.size() / 3;
			meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
			PxPhysics& physics = engine.getPhysicsBase();
			PxCooking& cooking = engine.getPhysicsCooking();
			//if (!cooking->validateTriangleMesh(meshDesc)) {
			//	PEX_WARN("Invalid terrain mesh");
			//}

			physx::PxTriangleMesh* mesh = cooking.createTriangleMesh(meshDesc, physics.getPhysicsInsertionCallback());
			physx::PxMaterial* material = physics.createMaterial(1.0f, 0.5f, 0.5f);
			data.shape = physics.createShape(physx::PxTriangleMeshGeometry(mesh), *material);
			mesh->release();
			material->release();
		}
	}
}