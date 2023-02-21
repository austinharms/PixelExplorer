#include "TerrainRenderMesh.h"

#include <new>
#include <string>
#include <unordered_map>

#include "PxeEngine.h"
#include "PxRigidStatic.h"
#include "SDL_timer.h"
#include "PxeVertexBuffer.h"
#include "PxeVertexBufferFormat.h"
#include "PxeVertexBufferAttrib.h"
#include "Log.h"
#include "GL/glew.h"
#include "PxMaterial.h"

namespace pixelexplorer {
	namespace terrain {
		const int8_t TriangleTable[0x100][0x10] = {
			{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
			{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
			{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
			{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
			{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
			{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
			{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
			{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
			{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
			{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
			{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
			{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
			{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
			{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
			{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
			{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
			{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
			{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
			{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
			{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
			{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
			{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
			{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
			{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
			{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
			{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
			{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
			{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
			{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
			{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
			{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
			{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
			{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
			{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
			{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
			{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
			{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
			{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
			{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
			{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
			{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
			{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
			{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
			{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
			{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
			{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
			{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
			{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
			{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
			{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
			{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
			{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
			{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
			{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
			{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
			{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
			{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
			{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
			{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
			{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
			{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
			{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
			{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
			{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
			{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
			{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
			{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
			{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
			{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
			{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
			{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
			{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
			{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
			{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
			{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
			{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
			{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
			{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
			{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
			{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
			{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
			{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
			{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
			{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
			{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
			{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
			{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
			{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
			{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
			{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
			{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
			{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
			{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
			{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
			{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
			{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
			{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
			{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
			{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
			{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
			{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
			{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
			{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
			{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
			{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
			{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
			{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
			{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
			{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
			{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
			{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
			{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
			{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
			{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
			{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
			{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
			{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
			{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
			{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
			{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
			{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
			{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
			{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
			{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
			{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
			{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
			{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
			{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
			{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
			{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
			{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
			{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
			{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
			{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
			{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
			{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
			{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
			{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
			{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
			{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
			{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
			{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
			{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
			{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
			{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
			{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
			{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
			{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
			{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
			{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
			{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
			{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
			{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
			{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
			{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
			{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
		};
		const glm::vec3 TriangleVertices[12] = {
			glm::vec3(0.5f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.5f),
			glm::vec3(0.5f, 0.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 0.5f),
			glm::vec3(0.5f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.5f),
			glm::vec3(0.5f, 1.0f, 1.0f),
			glm::vec3(0.0f, 1.0f, 0.5f),
			glm::vec3(0.0f, 0.5f, 0.0f),
			glm::vec3(1.0f, 0.5f, 0.0f),
			glm::vec3(1.0f, 0.5f, 1.0f),
			glm::vec3(0.0f, 0.5f, 1.0f)
		};
		const uint8_t TriangleColorMixingTable[12][2] = { {0,1}, {1,2}, {2,3}, {3,0}, {4,5}, {5,6}, {6,7}, {7,4}, {4,0}, {5,1}, {6,2}, {7,3} };
		const glm::vec3 TerrainColorTable[4] = { glm::vec3(1, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1) };

		TerrainRenderMesh::TerrainRenderMesh(pxengine::PxeRenderMaterialInterface& chunkMaterial) : pxengine::PxeStaticPhysicsRenderObject(chunkMaterial)
		{
			using namespace pxengine;
			_currentMeshDate = 0;
			_latestWorkingMeshDate = 0;
			_currentMeshType = NONE;
			_workPending = false;
			_hasMesh = false;
			_meshIndexBuffer = nullptr;
			_meshVertexArray = nullptr;
			memset(_chunks, 0, sizeof(_chunks));

			physx::PxPhysics& physics = pxengine::pxeGetEngine().getPhysicsBase();
			physx::PxTransform pxTransform(physx::PxIdentity);
			physx::PxRigidStatic* actor = physics.createRigidStatic(pxTransform);
			setPhysicsActor(actor);

			_meshIndexBuffer = new PxeIndexBuffer(PxeIndexType::UNSIGNED_16BIT, nullptr, true);
			PxeVertexBufferFormat fmt;
			fmt.addAttrib(PxeVertexBufferAttrib{ PxeVertexBufferAttribType::FLOAT, 3, false, 0 });
			fmt.addAttrib(PxeVertexBufferAttrib{ PxeVertexBufferAttribType::FLOAT, 3, false, 0 });
			_meshVertexBuffer = new PxeVertexBuffer(fmt, nullptr, true);
			_meshVertexArray = new PxeVertexArray(true);
			_meshVertexArray->addVertexBuffer(*_meshVertexBuffer, 0, 0);
			_meshVertexArray->addVertexBuffer(*_meshVertexBuffer, 1, 1);
			_meshVertexArray->setIndexBuffer(_meshIndexBuffer);

			_meshIndexBuffer->initializeAsset();
			_meshVertexBuffer->initializeAsset();
			_meshVertexArray->initializeAsset();
		}

		TerrainRenderMesh::~TerrainRenderMesh()
		{
			unloadChunks();
			_meshIndexBuffer->drop();
			_meshVertexArray->drop();
			_meshVertexBuffer->drop();
		}

		void TerrainRenderMesh::loadChunks(const glm::i64vec3& chunkPos, TerrainManager& mgr)
		{
			unloadChunks();
			const glm::i64vec3 chunkOffsets[CHUNK_COUNT] = {
				glm::i64vec3(0, 0, 0), // CENTER
				glm::i64vec3(1, 0, 0), // RIGHT
				glm::i64vec3(0, 0, 1), // FORWARD
				glm::i64vec3(1, 0, 1), // RIGHT_FORWARD
				glm::i64vec3(0, 1, 0), // TOP_CENTER
				glm::i64vec3(1, 1, 0), // TOP_RIGHT
				glm::i64vec3(0, 1, 1), // TOP_FORWARD
				glm::i64vec3(1, 1, 1), // TOP_RIGHT_FORWARD
			};

			for (uint8_t i = 0; i < CHUNK_COUNT; ++i)
				_chunks[i] = mgr.getTerrainChunk(chunkPos + chunkOffsets[i]);
			positionMatrix = glm::mat4(1);
			positionMatrix = glm::scale(positionMatrix, glm::vec3(TerrainChunk::CHUNK_CELL_SIZE));
			glm::vec3 worldPos(glm::vec3(chunkPos) * (float)TerrainChunk::CHUNK_GRID_SIZE);
			positionMatrix = glm::translate(positionMatrix, worldPos);
			static_cast<physx::PxRigidStatic*>(getPhysicsActor())->setGlobalPose(physx::PxTransform(physx::PxVec3(worldPos.x, worldPos.y, worldPos.z)));
		}

		void TerrainRenderMesh::unloadChunks()
		{
			std::lock_guard lock(_meshWorkMutex);
			_currentMeshDate = 0;
			_latestWorkingMeshDate = 0;
			_latestWorkingMeshType = NONE;
			_currentMeshType = NONE;
			_workPending = false;
			_hasMesh = false;

			physx::PxRigidStatic* actor = static_cast<physx::PxRigidStatic*>(getPhysicsActor());
			physx::PxScene* scene = actor->getScene();
			uint32_t shapeCount;
			if (scene)scene->lockRead(__FILE__, __LINE__);
			if ((shapeCount = actor->getNbShapes())) {
				if (scene) {
					scene->unlockRead();
					scene->lockWrite(__FILE__, __LINE__);
				}

				physx::PxShape** oldShapes = static_cast<physx::PxShape**>(alloca(sizeof(physx::PxShape*) * shapeCount));
				shapeCount = actor->getShapes(oldShapes, shapeCount);
				for (uint32_t i = 0; i < shapeCount; ++i)
					actor->detachShape(*(oldShapes[i]));
				if (scene) scene->unlockWrite();
			}
			else if (scene) {
				scene->unlockRead();
			}

			for (uint8_t i = 0; i < CHUNK_COUNT; ++i)
			{
				if (_chunks[i]) {
					_chunks[i]->drop();
					_chunks[i] = nullptr;
				}
			}
		}

		bool TerrainRenderMesh::getMeshOutdated() const
		{
			//return _chunks[CENTER] && _chunks[CENTER]->getLastModified() > _latestWorkingMeshDate;
			for (uint8_t i = 0; i < CHUNK_COUNT; ++i)
			{
				if (_chunks[i] && _chunks[i]->getLastModified() >= _latestWorkingMeshDate)
					return true;
			}

			return false;
		}

		uint64_t TerrainRenderMesh::getCurrentMeshTimestamp() const
		{
			return _currentMeshDate;
		}

		uint64_t TerrainRenderMesh::getWorkingMeshTimestamp() const
		{
			return _latestWorkingMeshDate;
		}

		TerrainRenderMesh::MeshType TerrainRenderMesh::getCurrentMeshType() const
		{
			return _currentMeshType;
		}

		TerrainRenderMesh::MeshType TerrainRenderMesh::getWorkingMeshType() const
		{
			return _latestWorkingMeshType;
		}

		bool TerrainRenderMesh::getWorkPending() const
		{
			std::lock_guard lock(_meshWorkMutex);
			return _workPending;
		}

		void TerrainRenderMesh::setWorkPending()
		{
			std::lock_guard lock(_meshWorkMutex);
			_workPending = true;
		}

		void TerrainRenderMesh::resetWorkPending()
		{
			std::lock_guard lock(_meshWorkMutex);
			_workPending = false;
		}

		TerrainChunk::ChunkPoint TerrainRenderMesh::getRelativeChunkPoint(uint32_t x, uint32_t y, uint32_t z) const {
			glm::u32vec3 localSpace(x, y, z);
			uint8_t chunkIndex = 0;
			if (x >= TerrainChunk::CHUNK_GRID_SIZE) {
				localSpace.x -= TerrainChunk::CHUNK_GRID_SIZE;
				chunkIndex += 1;
			}

			if (y >= TerrainChunk::CHUNK_GRID_SIZE) {
				localSpace.y -= TerrainChunk::CHUNK_GRID_SIZE;
				chunkIndex += 4;
			}

			if (z >= TerrainChunk::CHUNK_GRID_SIZE) {
				localSpace.z -= TerrainChunk::CHUNK_GRID_SIZE;
				chunkIndex += 2;
			}

			TerrainChunk* chunk = _chunks[chunkIndex];
			if (!chunk) return 0;
			return chunk->getPoints()[localSpace.x + localSpace.z * TerrainChunk::CHUNK_GRID_SIZE + localSpace.y * TerrainChunk::CHUNK_LAYER_POINT_COUNT];
		}

		physx::PxShape* TerrainRenderMesh::buildPhysicsShape(const std::vector<uint16_t>& indices, const std::vector<glm::vec3>& vertices) const
		{
			using namespace physx;
			if (vertices.size() == 0 || indices.size() == 0) return nullptr;
			PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = vertices.size()/2;
			meshDesc.points.data = &(vertices[0]);
			meshDesc.points.stride = sizeof(float) * 6;
			meshDesc.triangles.data = &(indices[0]);
			meshDesc.triangles.stride = sizeof(uint16_t) * 3;
			meshDesc.triangles.count = indices.size() / 3;
			meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
			PxPhysics& physics = pxengine::pxeGetEngine().getPhysicsBase();
			PxCooking& cooking = pxengine::pxeGetEngine().getPhysicsCooking();
			//if (!cooking->validateTriangleMesh(meshDesc)) {
			//	PEX_WARN("Invalid terrain mesh");
			//}

			physx::PxTriangleMesh* mesh = cooking.createTriangleMesh(meshDesc, physics.getPhysicsInsertionCallback());
			physx::PxMaterial* material = physics.createMaterial(1.0f, 0.5f, 0.5f);
			physx::PxShape* shape = physics.createShape(physx::PxTriangleMeshGeometry(mesh), *material);
			mesh->release();
			material->release();
			return shape;
		}

		void TerrainRenderMesh::buildTerrainMesh(std::vector<uint16_t>& indices, std::vector<glm::vec3>& vertices) const
		{
			std::unordered_map<glm::vec3, uint16_t> verticiesMap;
			for (uint32_t y = 0; y < TerrainChunk::CHUNK_GRID_SIZE; ++y) {
				for (uint32_t z = 0; z < TerrainChunk::CHUNK_GRID_SIZE; ++z) {
					for (uint32_t x = 0; x < TerrainChunk::CHUNK_GRID_SIZE; ++x) {
						uint8_t triIndex = 0;
						TerrainChunk::ChunkPoint points[8] = {
							getRelativeChunkPoint(x, y, z),
							getRelativeChunkPoint(x + 1, y, z),
							getRelativeChunkPoint(x + 1, y, z + 1),
							getRelativeChunkPoint(x, y, z + 1),
							getRelativeChunkPoint(x, y + 1, z),
							getRelativeChunkPoint(x + 1, y + 1, z),
							getRelativeChunkPoint(x + 1, y + 1, z + 1),
							getRelativeChunkPoint(x, y + 1, z + 1)
						};

						if (points[0]) triIndex |= 0x01;
						if (points[1]) triIndex |= 0x02;
						if (points[2]) triIndex |= 0x04;
						if (points[3]) triIndex |= 0x08;
						if (points[4]) triIndex |= 0x10;
						if (points[5]) triIndex |= 0x20;
						if (points[6]) triIndex |= 0x40;
						if (points[7]) triIndex |= 0x80;

						const int8_t* triPoints = TriangleTable[triIndex];
						for (uint8_t i = 0; i < 16; ++i) {
							if (triPoints[i] == -1) break;
							glm::vec3 vertex(glm::vec3(x, y, z) + TriangleVertices[triPoints[i]]);
							auto pair = verticiesMap.emplace(vertex, static_cast<uint16_t>(vertices.size() / 2));
							if (pair.second) {
								vertices.emplace_back(vertex);
								const uint8_t* colorMix = TriangleColorMixingTable[triPoints[i]];
								glm::vec3 color;
								if (points[colorMix[0]] && points[colorMix[1]]) {
									color = TerrainColorTable[points[colorMix[0]]] / 2.0f + TerrainColorTable[points[colorMix[1]]] / 2.0f;
								}
								else if (points[colorMix[0]]) {
									color = TerrainColorTable[points[colorMix[0]]];
								}
								else {
									color = TerrainColorTable[points[colorMix[1]]];
								}

								vertices.emplace_back(color);
							}

							indices.emplace_back(pair.first->second);
						}
					}
				}
			}
		}

		void TerrainRenderMesh::buildTerrainMeshFast(std::vector<uint16_t>& indices, std::vector<glm::vec3>& vertices) const
		{
			for (uint32_t y = 0; y < TerrainChunk::CHUNK_GRID_SIZE; ++y) {
				for (uint32_t z = 0; z < TerrainChunk::CHUNK_GRID_SIZE; ++z) {
					for (uint32_t x = 0; x < TerrainChunk::CHUNK_GRID_SIZE; ++x) {
						uint8_t triIndex = 0;
						TerrainChunk::ChunkPoint points[8] = {
							getRelativeChunkPoint(x, y, z),
							getRelativeChunkPoint(x + 1, y, z),
							getRelativeChunkPoint(x + 1, y, z + 1),
							getRelativeChunkPoint(x, y, z + 1),
							getRelativeChunkPoint(x, y + 1, z),
							getRelativeChunkPoint(x + 1, y + 1, z),
							getRelativeChunkPoint(x + 1, y + 1, z + 1),
							getRelativeChunkPoint(x, y + 1, z + 1)
						};

						if (points[0]) triIndex |= 0x01;
						if (points[1]) triIndex |= 0x02;
						if (points[2]) triIndex |= 0x04;
						if (points[3]) triIndex |= 0x08;
						if (points[4]) triIndex |= 0x10;
						if (points[5]) triIndex |= 0x20;
						if (points[6]) triIndex |= 0x40;
						if (points[7]) triIndex |= 0x80;

						const int8_t* triPoints = TriangleTable[triIndex];
						for (uint8_t i = 0; i < 16; ++i) {
							if (triPoints[i] == -1) break;
							glm::vec3 vertex(glm::vec3(x, y, z) + TriangleVertices[triPoints[i]]);
							indices.emplace_back(vertices.size() / 2);
							vertices.emplace_back(vertex);
							const uint8_t* colorMix = TriangleColorMixingTable[triPoints[i]];
							glm::vec3 color(0);
							if (points[colorMix[0]] && points[colorMix[1]]) {
								color = TerrainColorTable[points[colorMix[0]]] / 2.0f + TerrainColorTable[points[colorMix[1]]] / 2.0f;
							}
							else if (points[colorMix[0]]) {
								color = TerrainColorTable[points[colorMix[0]]];
							}
							else {
								color = TerrainColorTable[points[colorMix[1]]];
							}

							vertices.emplace_back(color);
						}
					}
				}
			}
		}

		void TerrainRenderMesh::updateRenderMesh(const std::vector<uint16_t>& indices, const std::vector<glm::vec3>& vertices)
		{
			pxengine::PxeBuffer* indexData = new pxengine::PxeBuffer(indices.size() * sizeof(uint16_t));
			pxengine::PxeBuffer* vertexData = new pxengine::PxeBuffer(vertices.size() * sizeof(glm::vec3));
			if (indices.size() && vertices.size()) {
				memcpy(indexData->getBuffer(), &(indices[0]), indexData->getSize());
				memcpy(vertexData->getBuffer(), &(vertices[0]), vertexData->getSize());
				_hasMesh = true;
			}
			else {
				_hasMesh = false;
			}

			_meshIndexBuffer->bufferData(*indexData);
			_meshVertexBuffer->bufferData(*vertexData);
			indexData->drop();
			vertexData->drop();
		}

		void TerrainRenderMesh::rebuildMesh(MeshType type)
		{
			_meshWorkMutex.lock();
			_workPending = false;
			uint64_t buildDate = SDL_GetTicks64();
			_latestWorkingMeshDate = buildDate;
			_latestWorkingMeshType = type;
			_meshWorkMutex.unlock();
			std::vector<uint16_t> indices;
			std::vector<glm::vec3> vertices;
			if (type == FAST) {
				buildTerrainMeshFast(indices, vertices);
			}
			else {
				buildTerrainMesh(indices, vertices);
			}

			physx::PxShape* shape = buildPhysicsShape(indices, vertices);
			std::lock_guard lock(_meshWorkMutex);
			if (_currentMeshDate > buildDate) {
				if (shape) shape->release();
				return;
			}

			physx::PxRigidStatic* actor = static_cast<physx::PxRigidStatic*>(getPhysicsActor());
			physx::PxScene* scene = actor->getScene();
			if (scene) scene->lockWrite(__FILE__, __LINE__);
			uint32_t shapeCount;
			if ((shapeCount = actor->getNbShapes())) {
				physx::PxShape** oldShapes = static_cast<physx::PxShape**>(alloca(sizeof(physx::PxShape*) * shapeCount));
				shapeCount = actor->getShapes(oldShapes, shapeCount);
				for (uint32_t i = 0; i < shapeCount; ++i)
					actor->detachShape(*(oldShapes[i]));
			}

			if (shape) actor->attachShape(*shape);
			if (scene) scene->unlockWrite();
			if (shape) shape->release();
			updateRenderMesh(indices, vertices);
			_currentMeshDate = buildDate;
			_currentMeshType = type;
			//PEX_INFO(("Terrain Mesh Rebuild took " + std::to_string(SDL_GetTicks64() - buildDate) + "ms").c_str());
		}

		void TerrainRenderMesh::onRender()
		{
			if (!_hasMesh || _meshVertexArray->getAssetStatus() != pxengine::PxeGLAssetStatus::INITIALIZED) return;
			_meshVertexArray->bind();
			if (_meshVertexArray->getBindingError()) {
				_meshVertexArray->unbind();
				return;
			}

			glDrawElements(GL_TRIANGLES, _meshIndexBuffer->getIndexCount(), (uint32_t)_meshIndexBuffer->getIndexType(), nullptr);
		}

		glm::i64vec3 TerrainRenderMesh::getChunkPosition() const
		{
			if (_chunks[CENTER]) return _chunks[CENTER]->getPosition();
			return glm::i64vec3(0);
		}
	}
}
