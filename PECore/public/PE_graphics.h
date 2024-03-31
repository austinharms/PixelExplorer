#ifndef PE_GRAPHICS_H_
#define PE_GRAPHICS_H_
#include "PE_defines.h"
#include "PE_graphics_types.h"

// Functions using this will be generated in PE_generated_graphics_api.h header file
#define PE_GRAPHICS_API PE_API
// Empty macro used to created functions that are only generated in the PE_generated_graphics_api.h header file
#define PE_GRAPHICS_ONLY_API(...) 

// Sets up the graphics adapter, returns 0 on success
// Note: must be run on the events aka main thread
// positive return values are reserved for PE_ERROR_* codes
PE_EXTERN_C PE_NODISCARD PE_API int PE_CALL PE_InitGraphics();

// Quits the graphics adapter created by Init()
// Note: must be run on the events aka main thread
PE_EXTERN_C PE_API void PE_CALL PE_QuitGraphics();

PE_GRAPHICS_ONLY_API(PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_INTERNAL_InitGraphicsSystem();)
PE_GRAPHICS_ONLY_API(PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_INTERNAL_QuitGraphicsSystem();)
PE_EXTERN_C PE_NODISCARD PE_GRAPHICS_API SDL_Window* PE_CALL PE_CreateWindow(char* title, int width, int height, Uint32 flags);
PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_DestroyWindow(SDL_Window* window);
PE_EXTERN_C PE_NODISCARD PE_GRAPHICS_API PE_Shader* PE_CALL PE_LoadShader(const char* name);
PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_UnloadShader(PE_Shader* shader);
PE_EXTERN_C PE_NODISCARD PE_GRAPHICS_API PE_RenderMesh* PE_CALL PE_CreateRenderMesh(void* vertices, size_t vertices_size, void* indices, size_t indices_size, PE_IndexType index_type, PE_MeshFormatAttrib* format_attributes, size_t attribute_count);
PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_DestroyRenderMesh(PE_RenderMesh* mesh);
PE_EXTERN_C PE_NODISCARD PE_GRAPHICS_API PE_GraphicsCommandQueue* PE_CALL PE_CreateGraphicsCommandQueue();
PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_ClearGraphicsCommandQueue(PE_GraphicsCommandQueue* queue);
PE_EXTERN_C PE_GRAPHICS_API void PE_CALL PE_DestroyGraphicsCommandQueue(PE_GraphicsCommandQueue* queue);
PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_RenderToWindow(SDL_Window* target_window, PE_GraphicsCommandQueue** queues, size_t queue_count);
PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_GraphicsCommandSetShader(PE_GraphicsCommandQueue* queue, PE_Shader* shader);
PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_GraphicsCommandSetMesh(PE_GraphicsCommandQueue* queue, PE_RenderMesh* mesh);
PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_GraphicsCommandDrawMesh(PE_GraphicsCommandQueue* queue, size_t index_count, size_t index_offset);
PE_EXTERN_C PE_GRAPHICS_API int PE_CALL PE_GraphicsCommandCull(PE_GraphicsCommandQueue* queue, PE_CullMode mode);
#endif // !PE_GRAPHICS_H_ 
