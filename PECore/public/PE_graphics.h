#ifndef PE_GRAPHICS_H_
#define PE_GRAPHICS_H_
#include "PE_defines.h"
#include "PE_referenced.h"
#include "PE_errors.h"

struct SDL_Window;

namespace pe {
	class PE_API Window : public Referenced {
	public:
		PE_NODISCARD virtual SDL_Window* GetSDLHandle() const;
	};

	namespace graphics {
		enum class MeshAttribType
		{
			FLOAT_ATTRIB = 0,
			HALF_FLOAT_ATTRIB,
			INT8_ATTRIB,
			UINT8_ATTRIB,
			INT16_ATTRIB,
			UINT16_ATTRIB,
			INT32_ATTRIB,
			UINT32_ATTRIB,
			ENUM_VALUE_COUNT
		};

		enum class IndexType
		{
			LINEAR_INDEX = 0, // No mesh indices. assumes triangles are to be drawn in the order provided in the vertex buffer
			U8_INDEX,
			U16_INDEX,
			U32_INDEX,
			ENUM_VALUE_COUNT
		};

		enum class CullMode {
			CULL_BACK = 0,
			CULL_FRONT,
			CULL_FRONT_BACK,
			CULL_NONE,
			ENUM_VALUE_COUNT
		};

		enum class RenderSwapMode {
			SWAP_IMMEDIATE = 0,
			SWAP_VSYNC,
			ENUM_VALUE_COUNT
		};

		enum class RenderPass {
			UNLIT_PASS,
			ENUM_VALUE_COUNT
		};

		struct MeshFormatAttrib {
			uint8_t attrib_type;
			uint8_t location;
			uint8_t size;
			uint8_t normalized;
		};

		struct MeshFormat {
			MeshFormatAttrib vertex_attribs[16];
			uint8_t index_type;
			uint8_t packed;
		};

		class PE_API Shader : public Referenced {};
		class PE_API RenderMesh : public Referenced {};
		class PE_API CommandQueue : public Referenced {
		public:
			virtual void Clear() = 0;
			virtual ErrorCode SetShader(Shader* shader) = 0;
			virtual ErrorCode SetMeshFormat(MeshFormat* format) = 0;
			virtual ErrorCode SetMesh(RenderMesh* mesh) = 0;
			virtual ErrorCode DrawMesh(size_t index_count, size_t index_offset) = 0;
			virtual ErrorCode Cull(CullMode mode) = 0;
		};

		class PE_API GraphicsAPI {
		public:
			PE_NODISCARD virtual Window* CreateWindow(char* title, int width, int height, Uint32 flags, ErrorCode* err_out) = 0;
			PE_NODISCARD virtual Shader* LoadShader(const char* name, ErrorCode* err_out) = 0;
			PE_NODISCARD virtual RenderMesh* CreateRenderMesh(void* vertices, size_t vertices_size, void* indices, size_t indices_size, ErrorCode* err_out) = 0;
			PE_NODISCARD virtual CommandQueue* CreateCommandQueue(ErrorCode* err_out) = 0;
			virtual ErrorCode RenderToWindow(Window* target_window, CommandQueue** queues, size_t queue_count) = 0;
			virtual ~GraphicsAPI() = default;
			PE_NOCOPY(GraphicsAPI);

		protected:
			GraphicsAPI() = default;
		};
	}
}
#endif // !PE_GRAPHICS_H_ 

//#ifndef PE_GRAPHICS_H_
//#define PE_GRAPHICS_H_
//#include "PE_defines.h"
//#include "PE_graphics_types.h"
//
//// Functions using this will be generated in PE_generated_graphics_api.h header file
//#define PE_GRAPHICS_API PE_API
//// Empty macro used to created functions that are only generated in the PE_generated_graphics_api.h header file
//#define PE_GRAPHICS_ONLY_API(...) 
//
//// Sets up the graphics adapter, returns 0 on success
//// Note: must be run on the events aka main thread
//// positive return values are reserved for PE_ERROR_* codes
//PE_EXTERN_C PE_NODISCARD PE_API int PE_CALL PE_InitGraphics();
//
//// Quits the graphics adapter created by Init()
//// Note: must be run on the events aka main thread
//PE_EXTERN_C PE_API void PE_CALL PE_QuitGraphics();
//
//PE_GRAPHICS_ONLY_API(PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_INTERNAL_InitGraphicsSystem();)
//PE_GRAPHICS_ONLY_API(PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_INTERNAL_QuitGraphicsSystem();)
//PE_EXTERN_C PE_NODISCARD PE_GRAPHICS_API PE_Window* PE_CALL PE_CreateWindow(char* title, int width, int height, Uint32 flags);
//PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_DestroyWindow(PE_Window* window);
//PE_EXTERN_C PE_NODISCARD PE_GRAPHICS_API PE_Shader* PE_CALL PE_LoadShader(const char* name);
//PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_UnloadShader(PE_Shader* shader);
//PE_EXTERN_C PE_NODISCARD PE_GRAPHICS_API PE_RenderMesh* PE_CALL PE_CreateRenderMesh(void* vertices, size_t vertices_size, void* indices, size_t indices_size);
//PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_DestroyRenderMesh(PE_RenderMesh* mesh);
//PE_EXTERN_C PE_NODISCARD PE_GRAPHICS_API PE_GraphicsCommandQueue* PE_CALL PE_CreateGraphicsCommandQueue();
//PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_ClearGraphicsCommandQueue(PE_GraphicsCommandQueue* queue);
//PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_DestroyGraphicsCommandQueue(PE_GraphicsCommandQueue* queue);
//PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_RenderToWindow(PE_Window* target_window, PE_GraphicsCommandQueue** queues, size_t queue_count);
//PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_GraphicsCommandSetShader(PE_GraphicsCommandQueue* queue, PE_Shader* shader);
//PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_GraphicsCommandSetMeshFormat(PE_GraphicsCommandQueue* queue, PE_MeshFormat* format);
//PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_GraphicsCommandSetMesh(PE_GraphicsCommandQueue* queue, PE_RenderMesh* mesh);
//PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_GraphicsCommandDrawMesh(PE_GraphicsCommandQueue* queue, size_t index_count, size_t index_offset);
//PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_GraphicsCommandCull(PE_GraphicsCommandQueue* queue, PE_CullMode mode);
//#endif // !PE_GRAPHICS_H_ 
