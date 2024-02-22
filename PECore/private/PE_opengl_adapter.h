#ifndef PE_OPENGL_ADAPTER_H_
#define PE_OPENGL_ADAPTER_H_
#include "PE_defines.h"
#include "PE_graphics_adapter.h"

#define PE_GRAPHICS_API(rc, fn, params, args, ret)	\
	PE_EXTERN_C rc PE_CALL fn##_OGL params;
#include "PE_graphics_api.h"
#undef PE_GRAPHICS_API

//PE_NODISCARD PE_EXTERN_C int PE_CALL PE_InitGraphics_OGL(PE_DGAPI_GraphicsJumpTable* jmpTable);
//PE_NODISCARD PE_EXTERN_C SDL_Window* PE_CALL PE_CreateWindow(char* title, int width, int height, Uint32 flags)

#endif // !PE_OPENGL_ADAPTER_H_
