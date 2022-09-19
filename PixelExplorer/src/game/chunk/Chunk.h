#include <stdint.h>

#include "common/RefCount.h"
#include "game/block/Block.h"
#include "ChunkRenderMesh.h"
#include "glm/vec3.hpp"

#ifndef PIXELEXPLORER_GAME_CHUNK_CHUNK_H_
#define PIXELEXPLORER_GAME_CHUNK_CHUNK_H_
namespace pixelexplorer::game::chunk {
	class Chunk : public RefCount
	{
	public:
		static const uint32_t CHUNK_SIZE = 25;
		static const uint32_t LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
		static const uint32_t BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;

		inline Chunk(ChunkRenderMesh& renderMesh, const glm::i32vec3& position) : _renderMesh(renderMesh) {
			_renderMesh.grab();
			_position = position;
			memset(_blocks, 0, sizeof(_blocks));
			_renderMesh.setPosition(glm::vec3(_position.x * (int32_t)CHUNK_SIZE, _position.y * (int32_t)CHUNK_SIZE, _position.z * (int32_t)CHUNK_SIZE));
		}

		inline virtual ~Chunk() {
			_renderMesh.drop();
		}

		inline ChunkRenderMesh& getRenderMesh() const { return _renderMesh; }

		inline const block::Block& getBlock(uint32_t blockIndex) const { return _blocks[blockIndex]; }

		inline block::Block* getBlockPtr() const {
			return (block::Block*)_blocks;
		}

		inline const glm::i32vec3& getPosition() const { return _position; }

	private:
		ChunkRenderMesh& _renderMesh;
		block::Block _blocks[BLOCK_COUNT];
		glm::i32vec3 _position;
	};
}
#endif // !PIXELEXPLORER_GAME_CHUNK_H_
