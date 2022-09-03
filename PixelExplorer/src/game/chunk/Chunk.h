#include <stdint.h>

#include "RefCount.h"
#include "game/block/Block.h"
#include "ChunkRenderMesh.h"
#include "glm/vec3.hpp"

#ifndef PIXELEXPLORER_GAME_CHUNK_H_
#define PIXELEXPLORER_GAME_CHUNK_H_
namespace pixelexplorer::game::chunk {
	class Chunk : public RefCount
	{
	public:
		static const uint32_t CHUNK_SIZE = 25;
		static const uint32_t LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
		static const uint32_t BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;

		inline Chunk(ChunkRenderMesh* renderMesh, const glm::i32vec3& position) {
			renderMesh->grab();
			_renderMesh = renderMesh;
			_position = position;
			for (uint32_t i = 0; i < BLOCK_COUNT; ++i) {
				_blocks[i].Id = 1;
			}

			//for (uint32_t i = 0; i < 1; ++i) {
			//	_blocks[i].Id = 1;
			//}

			_renderMesh->setPosition(glm::vec3(_position.x * CHUNK_SIZE, _position.y * CHUNK_SIZE, _position.z * CHUNK_SIZE));
		}

		inline virtual ~Chunk() {
			_renderMesh->drop();
			_renderMesh = nullptr;
		}

		inline ChunkRenderMesh* getRenderMesh() const { return _renderMesh; }

		inline const block::Block& getBlock(uint32_t blockIndex) const { return _blocks[blockIndex]; }

	private:
		ChunkRenderMesh* _renderMesh;
		block::Block _blocks[BLOCK_COUNT];
		glm::i32vec3 _position;
	};
}
#endif // !PIXELEXPLORER_GAME_CHUNK_H_
