#include "PE_opengl_adapter.h"
#include "PE_event_loop.h"
#include "PE_log.h"
#include "GL/glew.h"
#include "SDL_video.h"
#include <thread>

#define PE_OGL_CTX_KEY PE_TEXT("PE_GLCTX")

struct PE_CreateWindowData_OGL {
	char* title;
	int width;
	int height;
	Uint32 flags;
};


// Returns 0 on success and a negative value on error
// Error Codes:
//		0: No Error
int PE_InitGraphics_OGL(PE_DGAPI_GraphicsJumpTable* jmpTable) {
#define PE_GRAPHICS_API(rc, fn, params, args, ret) jmpTable->##fn = fn##_OGL;
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API
	PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Init OpenGL graphics adapter"));
	return 0;
}

static void* PE_CreateWindowFunc_OGL(void* windowData) {
	PE_CreateWindowData_OGL* data = static_cast<PE_CreateWindowData_OGL*>(windowData);
	SDL_Window* window = SDL_CreateWindow(data->title, data->width, data->height, data->flags);
	if (!window) {
		PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window: %s"), SDL_GetError());
		return nullptr;
	}

	SDL_GLContext ctx = SDL_GL_CreateContext(window);
	if (!ctx) {
		PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create OpenGL context: %s"), SDL_GetError());
		SDL_DestroyWindow(window);
		return nullptr;
	}

	// This technically only supports one OpenGL context at a time
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, GLEW failed to init: %") SDL_PRIs32 PE_TEXT(", %s"), res, glewGetErrorString(res));
		SDL_GL_DeleteContext(ctx);
		SDL_DestroyWindow(window);
		return nullptr;
	}

	SDL_PropertiesID props = SDL_GetWindowProperties(window);
	if (SDL_SetProperty(props, PE_OGL_CTX_KEY, ctx) != 0) {
		PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to set context property: %s"), SDL_GetError());
		SDL_GL_DeleteContext(ctx);
		SDL_DestroyWindow(window);
		return nullptr;
	}

	return window;
}

static void* PE_DestroyWindowFunc_OGL(void* window) {
	SDL_PropertiesID props = SDL_GetWindowProperties(static_cast<SDL_Window*>(window));
	SDL_GLContext ctx = SDL_GetProperty(props, PE_OGL_CTX_KEY, nullptr);
	if (ctx) {
		if (SDL_GL_DeleteContext(ctx) != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to delete OpenGL context: %s"), SDL_GetError());
		}
	}
	else {
		PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Destroyed window with missing context property"));
	}

	SDL_DestroyWindow(static_cast<SDL_Window*>(window));
	return nullptr;
}

static SDL_Window* PE_CreateWindow_OGL(char* title, int width, int height, Uint32 flags) {
	flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
	PE_CreateWindowData_OGL data{ title, width, height, flags };
	return static_cast<SDL_Window*>(PE_RunEventLoopFunction(PE_CreateWindowFunc_OGL, &data));
}

static void PE_DestroyWindow_OGL(SDL_Window* window) {
	if (window) {
		PE_RunEventLoopFunction(PE_DestroyWindowFunc_OGL, window);
	}
}

static PE_Shader* PE_LoadShader_OGL(const char* name) {
	return nullptr;
}

static void PE_UnloadShader_OGL(PE_Shader* shader) {

}
