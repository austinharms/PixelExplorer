#include <stdint.h>

#include "RefCount.h"
#include "game/block/BlockInstance.h"

#ifndef PIXELEXPLORER_GAME_CHUNK_H_
#define PIXELEXPLORER_GAME_CHUNK_H_
namespace pixelexplorer::game::chunk {
	class Chunk : public RefCount
	{
	public:
		static const uint32_t CHUNK_SIZE = 25;
		static const uint32_t LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;
		static const uint32_t BLOCK_COUNT = LAYER_SIZE * CHUNK_SIZE;

		Chunk();
		virtual ~Chunk();

	private:
		block::BlockInstance _blocks[BLOCK_COUNT];
	};
}
#endif // !PIXELEXPLORER_GAME_CHUNK_H_
