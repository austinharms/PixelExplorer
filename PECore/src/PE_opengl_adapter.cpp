#ifndef PE_DISABLE_OPENGL
#include "PE_opengl_adapter.h"
#include "PE_event_loop.h"
#include "PE_log.h"
#include "GL/glew.h"
#include "SDL_video.h"

// Should we create and use a worker thread and context to load OpenGL data
#ifndef PE_OGL_BACKGROUND_LOADING
#define PE_OGL_BACKGROUND_LOADING 0
#endif // !PE_OGL_BACKGROUND_LOADING

// Should all windows use one OpenGl context and render on the event loop aka main thread
// Note: this does not ensure only one context exists,
// if PE_OGL_BACKGROUND_LOADING is true it will still create it's own context
#ifndef PE_OGL_SINGLE_CONTEXT
#define PE_OGL_SINGLE_CONTEXT 1
#endif // !PE_OGL_SINGLE_CONTEXT

#define PE_OGL_CTX_KEY PE_TEXT("PE_GLCTX")

namespace pecore::pe_graphics::open_gl {
	struct PE_WindowParameters {
		char* title;
		int width;
		int height;
		Uint32 flags;
	};

#if PE_OGL_SINGLE_CONTEXT
	static SDL_GLContext s_primary_context = nullptr;
	static SDL_Window* s_active_context_window = nullptr;
#endif

#define PE_OGL_LoadFunction PE_RunEventLoopFunction

	// Helper PE_EventLoopFunction that creates a SDL_Window
	// This must be run on the PE event loop aka main thread
	static void* PE_CreateWindowFunc(void* windowData) {
		PE_WindowParameters* params = static_cast<PE_WindowParameters*>(windowData);
		SDL_Window* window = SDL_CreateWindow(params->title, params->width, params->height, params->flags);
		if (!window) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window: %s"), SDL_GetError());
			return nullptr;
		}

#if PE_OGL_BACKGROUND_LOADING
#error Write this
		//if (l_backgroundGLContext == nullptr) {
		//	l_backgroundGLContext = SDL_GL_CreateContext(window);
		//	if (!l_backgroundGLContext) {
		//		PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create background OpenGL context: %s"), SDL_GetError());
		//		SDL_DestroyWindow(window);
		//		return nullptr;
		//	}
		//}
#endif

#if PE_OGL_SINGLE_CONTEXT
		if (!s_primary_context) {
			s_active_context_window = window;
			s_primary_context = SDL_GL_CreateContext(window);
			if (!s_primary_context) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create OpenGL context: %s"), SDL_GetError());
				SDL_DestroyWindow(window);
				return nullptr;
			}
		}
#else
#error Write this
		//SDL_GLContext ctx = SDL_GL_CreateContext(window);
		//if (!ctx) {
		//	PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create OpenGL context: %s"), SDL_GetError());
		//	SDL_DestroyWindow(window);
		//	return nullptr;
		//}
#endif

		// This technically only supports one OpenGL context at a time
		// but we are assume all the context exist on one GPU and have the same function pointers
		GLenum res = glewInit();
		if (res != GLEW_OK) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, GLEW failed to init: %") SDL_PRIs32 PE_TEXT(", %s"), res, glewGetErrorString(res));
#if !PE_OGL_SINGLE_CONTEXT
#error Write this
			//SDL_GL_DeleteContext(ctx);
#endif
			SDL_DestroyWindow(window);
			return nullptr;
		}

#if !PE_OGL_SINGLE_CONTEXT
#error Write this
		//SDL_PropertiesID props = SDL_GetWindowProperties(window);
		//if (SDL_SetProperty(props, PE_OGL_CTX_KEY, ctx) != 0) {
		//	PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to set context property: %s"), SDL_GetError());
		//	SDL_GL_DeleteContext(ctx);
		//	SDL_DestroyWindow(window);
		//	return nullptr;
		//}
#endif

		return window;
	}

	// Helper PE_EventLoopFunction that destroys a SDL_Window
	// This must be run on the PE event loop aka main thread
	static void* PE_DestroyWindowFunc(void* window) {
		//SDL_PropertiesID props = SDL_GetWindowProperties(static_cast<SDL_Window*>(window));
		//SDL_GLContext ctx = SDL_GetProperty(props, PE_OGL_CTX_KEY, nullptr);
		//if (ctx) {
		//	if (SDL_GL_DeleteContext(ctx) != 0) {
		//		PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to delete OpenGL context: %s"), SDL_GetError());
		//	}
		//}
		//else {
		//	PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Destroyed window with missing context property"));
		//}

		SDL_DestroyWindow(static_cast<SDL_Window*>(window));
		return nullptr;
	}

	// Error Codes:
	//		 0: No Error
	//		-1: Failed to set SDL_GL_SHARE_WITH_CURRENT_CONTEXT Attribute
	OpenGLGraphicsAdapter::OpenGLGraphicsAdapter(int* retVal) : GraphicsAdapterInterface(retVal) {
#if !PE_OGL_SINGLE_CONTEXT || PE_OGL_BACKGROUND_LOADING
		if (SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, SDL_TRUE) != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_SHARE_WITH_CURRENT_CONTEXT: %s"), SDL_GetError());
			*retVal = -1;
		}
#endif

		*retVal = 0;
	}

	OpenGLGraphicsAdapter::~OpenGLGraphicsAdapter() {

	}

	SDL_Window* OpenGLGraphicsAdapter::CreateWindow(char* title, int width, int height, Uint32 flags) {
		flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
		PE_WindowParameters params{ title, width, height, flags };
		return static_cast<SDL_Window*>(PE_RunEventLoopFunction(PE_CreateWindowFunc, &params));
	}

	void OpenGLGraphicsAdapter::DestroyWindow(SDL_Window* window) {
		if (window) {
			PE_RunEventLoopFunction(PE_DestroyWindowFunc, window);
		}
	}

	PE_Shader* OpenGLGraphicsAdapter::LoadShader(const char* name) {
		return nullptr;
	}

	void OpenGLGraphicsAdapter::UnloadShader(PE_Shader* shader) {

	}
}
#endif