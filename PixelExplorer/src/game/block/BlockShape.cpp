#include "BlockShape.h"
namespace pixelexplorer::game::block {
	const float BlockShape::s_blockFaceVertices[6][12] = {
		// front
		{
			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
		},
		// back
		{
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
		},
		// left
		{
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f,  0.5f,
		},
		// right
		{
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
		},
		// top
		{
			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
		},
		// bottom
		{
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
		},
	};

	const uint32_t BlockShape::s_blockFaceIndices[6][6] = {
		// front
		{
			0, 2, 1,
			0, 3, 2
		},
		// back
		{
			0, 1, 2,
			0, 2, 3
		},
		// left
		{
			0, 1, 2,
			0, 2, 3
		},
		// right
		{
			0, 2, 1,
			0, 3, 2
		},
		// top
		{
			0, 1, 2,
			0, 2, 3
		},
		// bottom
		{
			0, 2, 1,
			0, 3, 2
		}
	};

	void BlockShape::loadFaceMesh(const FaceDirection dir, const BlockFaceDefinition& blockFace, const glm::vec3& positionOffset, uint32_t& indexOffset, uint32_t& vertexFloatOffset, const DataBuffer<uint32_t>& indexBuffer, const DataBuffer<float>& vertexBuffer) {
		if (dir == FaceDirection::NONE) return;
		uint8_t faceInt = (uint8_t)dir;

		uint32_t* indexBuf = indexBuffer.getBufferPtr();
		for (uint8_t index = 0; index < getFaceIndexCount(); ++index) {
			indexBuf[indexOffset++] = s_blockFaceIndices[faceInt][index] + (vertexFloatOffset / getFloatsPerVertex());
		}

		float* vertexBuf = vertexBuffer.getBufferPtr();
		for (uint8_t vertex = 0; vertex < getFaceVertexCount(); ++vertex) {
			vertexBuf[vertexFloatOffset++] = s_blockFaceVertices[faceInt][(vertex * getFloatsPerVertex()) + 0] + positionOffset.x;
			vertexBuf[vertexFloatOffset++] = s_blockFaceVertices[faceInt][(vertex * getFloatsPerVertex()) + 1] + positionOffset.y;
			vertexBuf[vertexFloatOffset++] = s_blockFaceVertices[faceInt][(vertex * getFloatsPerVertex()) + 2] + positionOffset.z;
		}
	}
}
