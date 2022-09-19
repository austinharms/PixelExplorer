#include "common/RefCount.h"
#include "Chunk.h"

#ifndef PIXELEXPLORER_GAME_CHUNK_FLATCHUNKGENERATOR_H_
#define PIXELEXPLORER_GAME_CHUNK_FLATCHUNKGENERATOR_H_
namespace pixelexplorer::game::chunk {
	class ChunkGenerator : public RefCount
	{
	public:
		inline ChunkGenerator() {}
		inline virtual ~ChunkGenerator() {}
		virtual Chunk& generateChunk(Chunk& chunk) const = 0;
	};
}
#endif // !PIXELEXPLORER_GAME_CHUNK_FLATCHUNKGENERATOR_H_
