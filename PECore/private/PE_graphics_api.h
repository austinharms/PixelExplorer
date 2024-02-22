// This file is included multiple times don't add an include guard
#ifndef PE_GRAPHICS_API
#error PE_GRAPHICS_API must be defined when including this file
// This is only here to make intellisense happy
#include "PE_defines.h"
#define PE_GRAPHICS_API(rc, fn, params, args, ret) \
	typedef rc (PE_CALL *PE_DGAPI_##fn) params; \
	PE_EXTERN_C static rc PE_CALL fn## params;
struct PE_DGAPI_GraphicsJumpTable;
struct SDL_Window;
#endif // !PE_GRAPHICS_API

PE_GRAPHICS_API(SDL_Window*, PE_CreateWindow, (char* title, int width, int height, Uint32 flags), (title, width, height, flags), return)
PE_GRAPHICS_API(void, PE_DestroyWindow, (SDL_Window* window), (window),)
PE_GRAPHICS_API(int, PE_InitGraphics, (PE_DGAPI_GraphicsJumpTable* jmpTable), (jmpTable), return)
