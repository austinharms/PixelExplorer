#include "ChunkGenerator.h"
#include "Chunk.h"
#include "../block/Block.h"
#include "glm/vec3.hpp"

#ifndef PIXELEXPLORER_GAME_CHUNK_CHUNKGENERATOR_H_
#define PIXELEXPLORER_GAME_CHUNK_CHUNKGENERATOR_H_
namespace pixelexplorer::game::chunk {
	class FlatChunkGenerator : public ChunkGenerator
	{
	public:
		FlatChunkGenerator(uint32_t groundBlockId, int32_t groundLevel = 0);
		virtual ~FlatChunkGenerator();
		Chunk& generateChunk(Chunk& chunk) const override;
		void setBlockAtPos(block::Block& block, const glm::i32vec3& blockPos, Chunk& chunk) const;

	private:
		const int32_t _groundLevel;
		const uint32_t _groundBlockId;
	};
}
#endif // !PIXELEXPLORER_GAME_CHUNK_CHUNKGENERATOR_H_
