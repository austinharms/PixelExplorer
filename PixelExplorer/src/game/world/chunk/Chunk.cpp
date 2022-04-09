#include "Chunk.h"

#include "../WorldScene.h"
#include "Logger.h"
#include "block/BlockDefinition.h"
#include "block/BlockShape.h"
#include "block/BlockSet.h"
#include "../World.h"
#include "physics/PhysicsScene.h"
#include "util/Direction.h"

namespace px::game::chunk {

	Chunk::Chunk(world::WorldScene* scene, glm::ivec3 positon) {
		_status = Chunk::Status::CREATED;
		_scene = scene;
		_scene->grab();
		//memset(_adjacentChunks, 0, sizeof(_adjacentChunks));
		_physicsObject = new physics::StaticPhysicsObject(_position, nullptr, nullptr, 3, _scene->getPhysicsScene());
		memset(_blocks, 0, sizeof(_blocks));
		_position = positon;
		_renderMesh = _scene->getWorld()->createChunkMesh();
		_renderMesh->setPosition(positon);
		_renderMesh->setActive(true);
		_status = Chunk::Status::LOADING;
		memset(_blocks, 1, sizeof(_blocks));
		_blocks[0] = 0;
		_status = Chunk::Status::LOADED;
	}

	Chunk::~Chunk() {
		unload();
	}

	void Chunk::updateMesh() {
		_blockMutex.lock();
		_status = Chunk::Status::UPDATING;
		BlockSet* blockSet = _scene->getWorld()->getBlockSet();
		blockSet->grab();

		uint32_t renderVertexCount = 0;
		uint32_t renderIndexCount = 0;
		uint32_t physicsVertexCount = 0;
		uint32_t physicsIndexCount = 0;
		glm::ivec3 pos(-1, 0, 0);

		for (int32_t i = 0; i < BLOCK_COUNT; ++i) {
			if (++pos.x == CHUNK_SIZE) {
				pos.x = 0;
				if (++pos.z == CHUNK_SIZE) {
					pos.z = 0;
					++pos.y;
				}
			}

			if (_blocks[i].getId() == 0) continue;
			const BlockDefinition* def = blockSet->getDefinitionById(_blocks[i].getId());

			for (int8_t j = 0; j < util::Direction::DIRECTION_COUNT; ++j) {
				Block* other = getBlock(pos - (glm::ivec3)util::Direction::fromIndex(j).getOpposite());
				if (other != nullptr && other->getId() != 0) continue;
				renderIndexCount += def->Shape->RenderIndexCount[j];
				renderVertexCount += def->Shape->RenderVertexCount[j] * 5;
				physicsIndexCount += def->Shape->PhysicsIndexCount[j];
				physicsVertexCount += def->Shape->PhysicsVertexCount[j] * 3;
			}
		}

		pos.x = -1;
		pos.y = 0;
		pos.z = 0;
		util::DataBuffer<float>* renderVertexBuffer = new util::DataBuffer<float>(renderVertexCount);
		util::DataBuffer<float>* physicsVertexBuffer = new util::DataBuffer<float>(physicsVertexCount);
		util::DataBuffer<uint32_t>* renderIndexBuffer = new util::DataBuffer<uint32_t>(renderIndexCount);
		util::DataBuffer<uint32_t>* physicsIndexBuffer = new util::DataBuffer<uint32_t>(physicsIndexCount);
		renderIndexCount = 0;
		renderVertexCount = 0;
		physicsVertexCount = 0;
		physicsIndexCount = 0;

		for (int32_t i = 0; i < BLOCK_COUNT; ++i) {
			if (++pos.x == CHUNK_SIZE) {
				pos.x = 0;
				if (++pos.z == CHUNK_SIZE) {
					pos.z = 0;
					++pos.y;
				}
			}

			if (_blocks[i].getId() == 0) continue;
			const BlockDefinition* def = blockSet->getDefinitionById(_blocks[i].getId());
			for (int8_t j = 0; j < util::Direction::DIRECTION_COUNT; ++j) {
				Block* other = getBlock(pos - (glm::ivec3)util::Direction::fromIndex(j).getOpposite());
				if (other != nullptr && other->getId() != 0) continue;

				for (uint8_t k = 0; k < def->Shape->RenderIndexCount[j]; ++k) {
					renderIndexBuffer->buffer[renderIndexCount++] = def->Shape->RenderIndices[j][k] + (renderVertexCount/5);
				}

				for (uint8_t k = 0; k < def->Shape->RenderVertexCount[j]; ++k) {
					renderVertexBuffer->buffer[renderVertexCount++] = def->Shape->RenderVertices[j][(k * 5) + 0] + (_position.x * CHUNK_SIZE) + pos.x;
					renderVertexBuffer->buffer[renderVertexCount++] = def->Shape->RenderVertices[j][(k * 5) + 1] + (_position.y * CHUNK_SIZE) + pos.y;
					renderVertexBuffer->buffer[renderVertexCount++] = def->Shape->RenderVertices[j][(k * 5) + 2] + (_position.z * CHUNK_SIZE) + pos.z;
					// TODO update uvs
					renderVertexBuffer->buffer[renderVertexCount++] = def->Shape->RenderVertices[j][(k * 5) + 3];
					renderVertexBuffer->buffer[renderVertexCount++] = def->Shape->RenderVertices[j][(k * 5) + 4];
				}


				for (uint8_t k = 0; k < def->Shape->PhysicsIndexCount[j]; ++k) {
					physicsIndexBuffer->buffer[physicsIndexCount++] = def->Shape->PhysicsIndices[j][k] + (physicsVertexCount/3);
				}

				for (uint8_t k = 0; k < def->Shape->PhysicsVertexCount[j]; ++k) {
					physicsVertexBuffer->buffer[physicsVertexCount++] = def->Shape->PhysicsVertices[j][(k * 3) + 0] + (_position.x * CHUNK_SIZE) + pos.x;
					physicsVertexBuffer->buffer[physicsVertexCount++] = def->Shape->PhysicsVertices[j][(k * 3) + 1] + (_position.y * CHUNK_SIZE) + pos.y;
					physicsVertexBuffer->buffer[physicsVertexCount++] = def->Shape->PhysicsVertices[j][(k * 3) + 2] + (_position.z * CHUNK_SIZE) + pos.z;
				}
			}
		}

		_physicsObject->updateMesh(physicsVertexBuffer, physicsIndexBuffer, 3);
		_renderMesh->updateBuffers(renderVertexBuffer, renderIndexBuffer);
		physicsIndexBuffer->drop();
		physicsVertexBuffer->drop();
		renderIndexBuffer->drop();
		renderVertexBuffer->drop();
		blockSet->drop();
		_status = Chunk::Status::LOADED;
		_blockMutex.unlock();
	}

	//void Chunk::updateAdjacents() {}

	void Chunk::unload() {
		if (_status <= Status::UNLOADING) return;
		_status = Status::UNLOADING;
		_scene->drop();
		_scene = nullptr;
		_physicsObject->drop();
		_physicsObject = nullptr;
		_renderMesh->setActive(false);
		_renderMesh->setDropFlag();
		_renderMesh->drop();
		_renderMesh = nullptr;
		_status = Status::UNLOADED;
	}

	//void Chunk::setPosition(glm::vec3 position) {
	//	_position = position;
	//	_renderMesh->setPosition(_position * CHUNK_SIZE);
	//}

	glm::vec3 Chunk::getPosition() const { return _position; }

	const Chunk::Status Chunk::getStatus() const { return _status; }

	//void Chunk::setScene(world::WorldScene* scene) {
	//	if (_scene != nullptr) _scene->drop();
	//	_scene = scene;
	//	if (_scene != nullptr) _scene->grab();
	//}

	ChunkRenderMesh* Chunk::getRenderMesh() const { return _renderMesh; }

	Block* Chunk::getBlock(glm::ivec3 localPos) const
	{
		if (localPos.x < 0 || localPos.y < 0 || localPos.z < 0 || localPos.x >= CHUNK_SIZE || localPos.y >= CHUNK_SIZE || localPos.z >= CHUNK_SIZE )
			return nullptr;
		return (Block*)&_blocks[localPos.x + (localPos.z * CHUNK_SIZE) + (localPos.y * LAYER_SIZE)];
	}
}  // namespace px::game::chunk