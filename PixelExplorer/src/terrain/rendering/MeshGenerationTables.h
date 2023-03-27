#ifndef PIXELEXPLORER_TERRAIN_MESH_GENERATION_TABLES_H_
#define PIXELEXPLORER_TERRAIN_MESH_GENERATION_TABLES_H_
#include <stdint.h>
#include "glm/vec3.hpp"

namespace pixelexplorer {
	namespace terrain {
		namespace data {
			extern const int8_t TriangleTable[0x100][0x0F];
			extern const int8_t TriangleNormalTable[0x100][0x0F];
			extern const glm::vec3 TriangleNormals[74];
			extern const glm::vec3 TriangleVertices[12];
			extern const uint8_t TriangleColorMixingTable[12][2];
		}
	}
}
#endif // !PIXELEXPLORER_TERRAIN_MESH_GENERATION_TABLES_H_
