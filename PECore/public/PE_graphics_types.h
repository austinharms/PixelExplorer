#ifndef PE_GRAPHICS_TYPES_H_
#define PE_GRAPHICS_TYPES_H_
#include "PE_defines.h"

struct SDL_Window;
typedef struct SDL_Window SDL_Window;

struct PE_Shader;
typedef struct PE_Shader PE_Shader;

struct PE_RenderMesh;
typedef struct PE_RenderMesh PE_RenderMesh;

struct PE_GraphicsCommandQueue;
typedef struct PE_GraphicsCommandQueue PE_GraphicsCommandQueue;

typedef enum PE_MeshAttribType
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
} PE_MeshAttribType;

typedef enum PE_IndexType
{
	PE_INDEX_U8 = 0,
	PE_INDEX_U16,
	PE_INDEX_U32,
	PE_INDEX_ENUM_VALUE_COUNT
} PE_IndexType;

typedef enum PE_CullMode {
	PE_CULL_BACK = 0,
	PE_CULL_FRONT,
	PE_CULL_FRONT_BACK,
	PE_CULL_NONE,
	PE_CULL_ENUM_VALUE_COUNT
} PE_CullMode;

typedef enum PE_RenderSyncMode {
	PE_SYNC_IMMEDIATE = 0,
	PE_SYNC_VSYNC,
	PE_SYNC_ENUM_VALUE_COUNT
} PE_RenderSyncMode;

typedef enum PE_RenderPass {
	PE_PASS_UNLIT,
	PE_PASS_ENUM_VALUE_COUNT
} PE_RenderPass;

struct PE_MeshFormatAttrib {
	PE_MeshAttribType type;
	int32_t location;
	int32_t size;
	int32_t normalized;
	int32_t stride;
	int32_t offset;
};
typedef struct PE_MeshFormatAttrib PE_MeshFormatAttrib;
#endif // !PE_GRAPHICS_TYPES_H_
