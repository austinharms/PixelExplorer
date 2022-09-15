#include "BlockShape.h"
namespace pixelexplorer::game::block {
	const float BlockShape::BLOCK_FACE_VERTICES[FACE_COUNT][FACE_FLOAT_COUNT] = {
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

	const uint32_t BlockShape::BLOCK_FACE_INDICES[FACE_COUNT][FACE_INDEX_COUNT] = {
		// front
		{
			0, 1, 2,
			0, 2, 3
		},
		// back
		{
			0, 2, 1,
			0, 3, 2
		},
		// left
		{
			0, 2, 1,
			0, 3, 2
		},
		// right
		{
			0, 1, 2,
			0, 2, 3
		},
		// top
		{
			0, 2, 1,
			0, 3, 2
		},
		// bottom
		{
			0, 1, 2,
			0, 2, 3
		}
	};
}
