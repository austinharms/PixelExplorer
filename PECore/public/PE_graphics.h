#ifndef PE_GRAPHICS_H_
#define PE_GRAPHICS_H_
#include "PE_defines.h"

// Functions using this will be generated in PE_generated_graphics_api.h header file
#define PE_GRAPHICS_API PE_API
// Empty macro used to created functions that are only generated in the PE_generated_graphics_api.h header file
#define PE_GRAPHICS_ONLY_API(...) 

struct SDL_Window;

namespace pecore::graphics {
	struct Shader {};

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
	PE_GRAPHICS_API Shader* PE_CALL LoadShader(const char* name);
	PE_NODISCARD PE_GRAPHICS_API void PE_CALL UnloadShader(Shader* shader);
}
#endif // !PE_GRAPHICS_H_ 
