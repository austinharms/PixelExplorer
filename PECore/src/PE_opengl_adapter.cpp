#include "PE_opengl_adapter.h"
#include "PE_event_loop.h"
#include "PE_log.h"

PE_EXTERN_C struct PE_CreateWindowData_OGL {
		char* title;
		int width;
		int height;
		Uint32 flags;
	};

PE_EXTERN_C static void* PE_CreateWindowFunc_OGL(void* windowData) {
	PE_CreateWindowData_OGL* data = static_cast<PE_CreateWindowData_OGL*>(windowData);
	return SDL_CreateWindow(data->title, data->width, data->height, data->flags);
}

PE_EXTERN_C static void* PE_DestroyWindowFunc_OGL(void* window) {
	SDL_DestroyWindow(static_cast<SDL_Window*>(window));
	return nullptr;
}

PE_EXTERN_C static PE_NODISCARD SDL_Window* PE_CALL PE_CreateWindow_OGL(char* title, int width, int height, Uint32 flags) {
	flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
	PE_CreateWindowData_OGL data{ title, width, height, flags };
	return static_cast<SDL_Window*>(PE_RunEventLoopFunction(PE_CreateWindowFunc_OGL, &data));
}

PE_EXTERN_C static void PE_CALL PE_DestroyWindow_OGL(SDL_Window* window) {
	if (window) {
		PE_RunEventLoopFunction(PE_DestroyWindowFunc_OGL);
	}
}

PE_EXTERN_C PE_NODISCARD int PE_CALL PE_InitGraphics_OGL(PE_DGAPI_GraphicsJumpTable* jmpTable) {
#define PE_GRAPHICS_API(rc, fn, params, args, ret) jmpTable->##fn = fn##_OGL;
#include "PE_graphics_api.h"
#undef PE_GRAPHICS_API

	PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Init OpenGL graphics adapter"));
	return 0;
}
