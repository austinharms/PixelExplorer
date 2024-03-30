#ifndef PE_GRAPHICS_H_
#define PE_GRAPHICS_H_
#include "PE_defines.h"
#include "PE_graphics_types.h"

// Functions using this will be generated in PE_generated_graphics_api.h header file
#define PE_GRAPHICS_API PE_API
// Empty macro used to created functions that are only generated in the PE_generated_graphics_api.h header file
#define PE_GRAPHICS_ONLY_API(...) 

namespace pecore::graphics {
	// Sets up the graphics adapter, returns 0 on success
	// Note: must be run on the events aka main thread
	// positive return values are reserved for PE_ERROR_* codes
	PE_NODISCARD PE_API int PE_CALL Init();

	// Quits the graphics adapter created by Init()
	// Note: must be run on the events aka main thread
	PE_API void PE_CALL Quit();

	PE_GRAPHICS_ONLY_API(PE_GRAPHICS_API int PE_CALL InitSystem();)
	PE_GRAPHICS_ONLY_API(PE_GRAPHICS_API void PE_CALL QuitSystem();)
	PE_NODISCARD PE_GRAPHICS_API SDL_Window* PE_CALL CreateWindow(char* title, int width, int height, Uint32 flags);
	PE_GRAPHICS_API void PE_CALL DestroyWindow(SDL_Window* window);
	PE_NODISCARD PE_GRAPHICS_API Shader* PE_CALL LoadShader(const char* name);
	PE_GRAPHICS_API void PE_CALL UnloadShader(Shader* shader);
	PE_NODISCARD PE_GRAPHICS_API RenderMesh* PE_CALL CreateMesh(void* vertices, size_t vertices_size, void* indices, size_t indices_size, IndexType index_type, MeshFormatAttrib* format_attributes, size_t attribute_count);
	PE_GRAPHICS_API void PE_CALL DestroyMesh(RenderMesh* mesh);
	PE_NODISCARD PE_GRAPHICS_API CommandQueue* PE_CALL CreateCommandQueue();
	PE_GRAPHICS_API void PE_CALL ClearCommandQueue(CommandQueue* queue);
	PE_GRAPHICS_API void PE_CALL DestroyCommandQueue(CommandQueue* queue);
	PE_GRAPHICS_API int PE_CALL RenderToWindow(SDL_Window* target_window, CommandQueue** queues, size_t queue_count);
	PE_GRAPHICS_API int PE_CALL CommandSetShader(CommandQueue* queue, Shader* shader);
	PE_GRAPHICS_API int PE_CALL CommandSetMesh(CommandQueue* queue, RenderMesh* mesh);
	PE_GRAPHICS_API int PE_CALL CommandDrawMesh(CommandQueue* queue, size_t index_count, size_t index_offset);
	PE_GRAPHICS_API int PE_CALL CommandCull(CommandQueue* queue, CullMode mode);
}
#endif // !PE_GRAPHICS_H_ 
