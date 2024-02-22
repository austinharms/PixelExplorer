#ifndef PE_GRAPHICS_H_
#define PE_GRAPHICS_H_
#include "PE_defines.h"
#include "SDL_video.h"
PE_EXTERN_C PE_NODISCARD PE_API SDL_Window* PE_CALL PE_CreateWindow(char* title, int width, int height, Uint32 flags);
PE_EXTERN_C PE_API void PE_CALL PE_DestroyWindow(SDL_Window* window);
#endif // !PE_GRAPHICS_H_
