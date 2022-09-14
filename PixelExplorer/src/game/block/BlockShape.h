#include <stdint.h>

#include "BlockFaceDefinition.h"
#include "FaceDirection.h"
#include "glm/vec3.hpp"
#include "common/DataBuffer.h"

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCKSHAPE_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCKSHAPE_H_
namespace pixelexplorer::game::block {
	class BlockShape
	{
	public:
		static const uint32_t VERTEX_FLOAT_COUNT = 3;
		static const uint32_t FACE_VERTEX_COUNT = 4;
		static const uint32_t FACE_FLOAT_COUNT = VERTEX_FLOAT_COUNT * FACE_VERTEX_COUNT;
		static const uint32_t FACE_INDEX_COUNT = 6;
		static const uint32_t FACE_COUNT = 6;
		static const float BLOCK_FACE_VERTICES[FACE_COUNT][FACE_FLOAT_COUNT];
		static const uint32_t BLOCK_FACE_INDICES[FACE_COUNT][FACE_INDEX_COUNT];

	private:
		inline BlockShape() {}
		inline ~BlockShape() {}
	};
}
#endif // !PIXELEXPLORER_GAME_BLOCK_BLOCKSHAPE_H_
