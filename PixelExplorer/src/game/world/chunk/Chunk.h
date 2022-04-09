#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <mutex>

#include "Chunk.fwd.h"
#include "../WorldScene.fwd.h"
#include "util/Direction.h"
#include "physics/StaticPhysicsObject.h"
#include "RefCounted.h"
#include "ChunkRenderMesh.h"
#include "block/Block.h"
#include "glm/vec3.hpp"

namespace px::game::chunk {
	class Chunk : public RefCounted {
	public:
		enum class Status : uint8_t {
			UNLOADED = 0,
			UNLOADING = 1,
			CREATED = 2,
			LOADING = 3,
			LOADED = 4,
			UPDATING = 5
		};

		static const int32_t CHUNK_SIZE = 25;
		static const int32_t LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
		static const int32_t BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;

		Chunk(world::WorldScene* scene, glm::ivec3 positon);
		virtual ~Chunk();
		void updateMesh();
		//void updateAdjacents();
		void unload();
		//void setPosition(glm::vec3 position);
		glm::vec3 getPosition() const;
		const Status getStatus() const;
		//void setScene(world::WorldScene* scene);
		ChunkRenderMesh* getRenderMesh() const;

	private:
		world::WorldScene* _scene;
		//Chunk* _adjacentChunks[util::Direction::DIRECTION_COUNT];
		physics::StaticPhysicsObject* _physicsObject;
		ChunkRenderMesh* _renderMesh;
		std::mutex _blockMutex;
		Block _blocks[BLOCK_COUNT];
		Status _status;
		glm::ivec3 _position;

		Block* getBlock(glm::ivec3 localPos) const;
	};
}  // namespace px::game::chunk
#endif  // !CHUNK_H
