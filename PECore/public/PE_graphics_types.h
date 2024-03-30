#ifndef PE_GRAPHICS_TYPES_H_
#define PE_GRAPHICS_TYPES_H_
#include "PE_defines.h"

struct SDL_Window;

namespace pecore::graphics {
	struct Shader {};
	struct RenderMesh {};
	struct CommandQueue {};

	enum MeshAttribType
	{
		PE_MESH_ATR_FLOAT = 0,
		PE_MESH_ATR_HALF_FLOAT,
		PE_MESH_ATR_INT8,
		PE_MESH_ATR_UINT8,
		PE_MESH_ATR_INT16,
		PE_MESH_ATR_UINT16,
		PE_MESH_ATR_INT32,
		PE_MESH_ATR_UINT32,
		PE_MESH_ATR_ENUM_VALUE_COUNT
	};

	enum IndexType
	{
		PE_INDEX_U8 = 0,
		PE_INDEX_U16,
		PE_INDEX_U32,
		PE_INDEX_ENUM_VALUE_COUNT
	};

	enum CullMode {
		PE_CULL_BACK = 0,
		PE_CULL_FRONT,
		PE_CULL_FRONT_BACK,
		PE_CULL_NONE,
		PE_CULL_ENUM_VALUE_COUNT
	};

	enum RenderSyncMode {
		PE_SYNC_IMMEDIATE = 0,
		PE_SYNC_VSYNC,
		PE_SYNC_ENUM_VALUE_COUNT
	};

	enum RenderPass {
		PE_PASS_UNLIT,
		PE_PASS_ENUM_VALUE_COUNT
	};

	struct MeshFormatAttrib {
		MeshAttribType type;
		int32_t location;
		int32_t size;
		int32_t normalized;
		int32_t stride;
		int32_t offset;
	};
}
#endif // !PE_GRAPHICS_TYPES_H_
