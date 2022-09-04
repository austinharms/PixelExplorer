#include <stdint.h>

#include "FaceDirection.h"
#include "glm/vec3.hpp"
#include "common/DataBuffer.h"

#ifndef PIXELEXPLORER_GAME_BLOCK_BLOCKSHAPE_H_
#define PIXELEXPLORER_GAME_BLOCK_BLOCKSHAPE_H_
namespace pixelexplorer::game::block {
	class BlockShape
	{
	public:

		static void loadFaceMesh(const FaceDirection dir, const glm::vec3& positionOffset, uint32_t& indexOffset, uint32_t& vertexFloatOffset, const DataBuffer<uint32_t>& indexBuffer, const DataBuffer<float>& vertexBuffer);

		inline static const uint32_t getFloatsPerVertex() { return 3; }

		inline static const uint32_t getFaceVertexCount() { return 4; }

		inline static const uint32_t getFaceIndexCount() { return 6; }

		inline static const uint32_t getFaceFloatCount() { return getFloatsPerVertex() * getFaceVertexCount(); }

	private:
		static const float s_blockFaceVertices[6][12];
		static const uint32_t s_blockFaceIndices[6][6];

		inline BlockShape() {}
		inline ~BlockShape() {}
	};
}
#endif // !PIXELEXPLORER_GAME_BLOCK_BLOCKSHAPE_H_
