#include "FlatChunkGenerator.h"

namespace pixelexplorer::game::chunk {
	FlatChunkGenerator::FlatChunkGenerator(uint32_t groundBlockId, int32_t groundLevel) : _groundLevel(groundLevel), _groundBlockId(groundBlockId) { }

	FlatChunkGenerator::~FlatChunkGenerator() { }

	Chunk& FlatChunkGenerator::generateChunk(Chunk& chunk) const
	{
		chunk.grab();
		glm::i32vec3 blockPos(0);
		glm::i32vec3 chunkOffset = chunk.getPosition() * glm::i32vec3(Chunk::CHUNK_SIZE);
		block::Block* blocks = chunk.getBlockPtr();
		for (uint32_t i = 0; i < Chunk::BLOCK_COUNT; ++i) {
			setBlockAtPos(blocks[i], blockPos + chunkOffset, chunk);

			if (++blockPos.x == Chunk::CHUNK_SIZE) {
				blockPos.x = 0;
				if (++blockPos.z == Chunk::CHUNK_SIZE) {
					blockPos.z = 0;
					++blockPos.y;
				}
			}
		}

		chunk.drop();
		return chunk;
	}

	void FlatChunkGenerator::setBlockAtPos(block::Block& block, const glm::i32vec3& blockPos, Chunk& chunk) const
	{
		if (blockPos.y <= _groundLevel) {
			block.Id = _groundBlockId;
		}
		else {
			block.Id = 0;
		}
	}
}
