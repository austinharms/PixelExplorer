#ifndef PE_GRAPHICS_H_
#define PE_GRAPHICS_H_
#include "PE_defines.h"
#include "SDL_video.h"
#include "PE_graphics_types.h"
// An empty macro that will prevent the function from being generated in PE_dgapi.h
#define PE_NO_DGAPI
// An empty macro that will prevent the function from being declared but, will allow it to be generated in PE_dgapi.h
#define PE_ONLY_DGAPI(func)

// Returns 0 on success and a negative value on error
PE_NO_DGAPI PE_EXTERN_C PE_NODISCARD PE_API int PE_CALL PE_InitGraphicsAdapter();
PE_EXTERN_C PE_NODISCARD PE_API SDL_Window* PE_CALL PE_CreateWindow(char* title, int width, int height, Uint32 flags);
PE_EXTERN_C PE_API void PE_CALL PE_DestroyWindow(SDL_Window* window);
PE_EXTERN_C PE_API PE_Shader* PE_CALL PE_LoadShader(const char* name);
PE_EXTERN_C PE_NODISCARD PE_API void PE_CALL PE_UnloadShader(PE_Shader* shader);
PE_ONLY_DGAPI(PE_EXTERN_C PE_NODISCARD PE_API int PE_CALL PE_InitGraphics(PE_DGAPI_GraphicsJumpTable* jmpTable);)
#endif // !PE_GRAPHICS_H_ 
