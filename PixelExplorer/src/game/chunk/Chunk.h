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
				_blocks[i].Id = 0;
			}

			_renderMesh->setPosition(_position);
		}

		inline virtual ~Chunk() {
			_renderMesh->drop();
			_renderMesh = nullptr;
		}

		inline ChunkRenderMesh* getRenderMesh() const { return _renderMesh; }

	private:
		ChunkRenderMesh* _renderMesh;
		block::Block _blocks[BLOCK_COUNT];
		glm::i32vec3 _position;
	};
}
#endif // !PIXELEXPLORER_GAME_CHUNK_H_
