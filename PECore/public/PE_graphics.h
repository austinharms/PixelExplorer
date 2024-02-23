#ifndef PE_GRAPHICS_H_
#define PE_GRAPHICS_H_
#include "PE_defines.h"
#include "PE_graphics_types.h"

// An empty macro that will prevent the function from being generated in PE_dgapi.h
#define PE_NO_DGAPI
namespace pecore::pe_graphics {
	// Returns 0 on success
	// Note: positive return values are reserved for PE_ERROR_* codes
	PE_NO_DGAPI PE_NODISCARD PE_API int PE_CALL PE_InitGraphicsAdapter();
	PE_NO_DGAPI PE_API void PE_CALL PE_QuitGraphicsAdapter();
	PE_NODISCARD PE_API SDL_Window* PE_CALL PE_CreateWindow(char* title, int width, int height, Uint32 flags);
	PE_API void PE_CALL PE_DestroyWindow(SDL_Window* window);
	PE_API PE_Shader* PE_CALL PE_LoadShader(const char* name);
	PE_NODISCARD PE_API void PE_CALL PE_UnloadShader(PE_Shader* shader);
}
#endif // !PE_GRAPHICS_H_ 
