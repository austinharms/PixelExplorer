#ifndef PE_DISABLE_OPENGL
#include "PE_opengl_adapter.h"
#include "PE_event_loop.h"
#include "PE_log.h"
#include "SDL_video.h"

#define PE_OGL_CTX_KEY PE_TEXT("PE_GLCTX")

namespace pecore::pe_graphics::open_gl {
	struct PE_WindowParameters {
		OpenGLGraphicsAdapter* self;
		char* title;
		int width;
		int height;
		Uint32 flags;
	};

#if !PE_OGL_SINGLE_CONTEXT
	struct PE_WindowContextData {
		GladGLContext ctx;
	};
#endif

	// Helper PE_EventLoopFunction that destroys a GladGLContext and SDL_GLContext
	// Note: it is expected that GladGLContext.userptr contains the SDL_GLContext
	static void* PE_DestroyGladContextFunc(void* ctx) {
		SDL_GL_DeleteContext(static_cast<GladGLContext*>(ctx)->userptr);
		memset(ctx, 0, sizeof(GladGLContext));
		return nullptr;
	}

	// Helper PE_EventLoopFunction that destroys a SDL_Window
	// This must be run on the PE event loop aka main thread
	static void* PE_DestroyWindowFunc(void* window) {
#if !PE_OGL_SINGLE_CONTEXT
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
#endif

		SDL_DestroyWindow(static_cast<SDL_Window*>(window));
		return nullptr;
	}

	// Error Codes:
	//		 0: No Error
	//		-1: Failed to set SDL_GL_SHARE_WITH_CURRENT_CONTEXT Attribute
	//		-2: Failed to set SDL_GL_DOUBLEBUFFER Attribute
	//		-3: Failed to set SDL_GL_CONTEXT_MAJOR_VERSION Attribute
	//		-4: Failed to set SDL_GL_CONTEXT_MINOR_VERSION Attribute
	//		-5: Failed to set SDL_GL_CONTEXT_PROFILE_MASK Attribute
	OpenGLGraphicsAdapter::OpenGLGraphicsAdapter(int* retVal) : GraphicsAdapterInterface(retVal) {
#if PE_OGL_SINGLE_CONTEXT
		memset(&primary_glad_ctx_, 0, sizeof(GladGLContext));
		active_ctx_window_ = nullptr;
#endif
#if PE_OGL_BACKGROUND_LOADING
		memset(&background_glad_ctx_, 0, sizeof(GladGLContext));
#endif
#if !PE_OGL_SINGLE_CONTEXT || PE_OGL_BACKGROUND_LOADING
		if (SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, SDL_TRUE) != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_SHARE_WITH_CURRENT_CONTEXT: %s"), SDL_GetError());
			*retVal = -1;
		}
#endif
		if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_DOUBLEBUFFER: %s"), SDL_GetError());
			*retVal = -2;
		}

		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_CONTEXT_MAJOR_VERSION: %s"), SDL_GetError());
			*retVal = -3;
		}

		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_CONTEXT_MINOR_VERSION: %s"), SDL_GetError());
			*retVal = -4;
		}

		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_CONTEXT_PROFILE_MASK: %s"), SDL_GetError());
			*retVal = -5;
		}

		*retVal = 0;
	}

	OpenGLGraphicsAdapter::~OpenGLGraphicsAdapter() {
#if PE_OGL_SINGLE_CONTEXT
		if (primary_glad_ctx_.userptr) {
			PE_RunEventLoopFunction(PE_DestroyGladContextFunc, &primary_glad_ctx_);
		}
#endif

#if PE_OGL_BACKGROUND_LOADING
		if (background_glad_ctx_.userptr) {
			PE_RunEventLoopFunction(PE_DestroyGladContextFunc, &background_glad_ctx_);
		}
#endif
	}

	SDL_Window* OpenGLGraphicsAdapter::CreateWindow(char* title, int width, int height, Uint32 flags) {
		flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
		PE_WindowParameters params{ this, title, width, height, flags };
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

	void* OpenGLGraphicsAdapter::PE_CreateWindowFunc(void* windowData) {
		PE_WindowParameters* params = static_cast<PE_WindowParameters*>(windowData);
		OpenGLGraphicsAdapter* this_ = params->self;
		SDL_Window* window = SDL_CreateWindow(params->title, params->width, params->height, params->flags);
		if (!window) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window: %s"), SDL_GetError());
			return nullptr;
		}

#if PE_OGL_BACKGROUND_LOADING
		if (this_->background_glad_ctx_.userptr == nullptr) {
			SDL_GLContext ctx = SDL_GL_CreateContext(window);
			if (!ctx) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create background OpenGL context: %s"), SDL_GetError());
				SDL_DestroyWindow(window);
				return nullptr;
			}

			int ver = gladLoadGLContext(&this_->background_glad_ctx_, SDL_GL_GetProcAddress);
			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Background GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
			this_->background_glad_ctx_.userptr = ctx;
		}
#endif

#if PE_OGL_SINGLE_CONTEXT
		if (this_->primary_glad_ctx_.userptr == nullptr) {
			this_->active_ctx_window_ = window;
			SDL_GLContext ctx = SDL_GL_CreateContext(window);
			if (!ctx) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create OpenGL context: %s"), SDL_GetError());
				SDL_DestroyWindow(window);
				return nullptr;
			}

			int ver = gladLoadGLContext(&this_->primary_glad_ctx_, SDL_GL_GetProcAddress);
			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Primary GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
			this_->primary_glad_ctx_.userptr = ctx;
		}
#else
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
}
#endif