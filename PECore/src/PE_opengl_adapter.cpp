#ifndef PE_DISABLE_OPENGL
#include "PE_opengl_adapter.h"
#include "PE_event_loop.h"
#include "PE_worker_thread.h"
#include "PE_log.h"
#include "PE_memory.h"
#include "PE_errors.h"
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
		SDL_Window* window;
		GladGLContext ctx;
		ThreadWorker worker;
	};
#endif

#if PE_OGL_BACKGROUND_LOADING
	struct PE_SetGLContextActiveFuncParams {
		SDL_Window* window;
		SDL_GLContext context;
	};

	static void* PE_SetGLContextActiveFunc(void* data) {
		PE_SetGLContextActiveFuncParams* params = static_cast<PE_SetGLContextActiveFuncParams*>(data);
		intptr_t ret = SDL_GL_MakeCurrent(params->window, params->context);
		if (ret != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set OpenGL context active: %s"), SDL_GetError());
		}

		return reinterpret_cast<void*>(ret);
	}
#endif

	// Helper PE_EventLoopFunction that destroys a GladGLContext and SDL_GLContext
	// Note: it is expected that GladGLContext.userptr contains the SDL_GLContext
	static void* PE_DestroyGladContextFunc(void* ctx) {
		if (SDL_GL_DeleteContext(static_cast<GladGLContext*>(ctx)->userptr) != 0) {
			PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to delete OpenGL context: %s"), SDL_GetError());
		}

		memset(ctx, 0, sizeof(GladGLContext));
		return nullptr;
	}

	// Helper PE_EventLoopFunction that destroys a SDL_Window
	// This must be run on the PE event loop aka main thread
	static void* PE_DestroyWindowFunc(void* window) {
#if !PE_OGL_SINGLE_CONTEXT
		PE_WindowContextData* ctx = nullptr;
		SDL_PropertiesID props = SDL_GetWindowProperties(static_cast<SDL_Window*>(window));
		if (props != 0) {
			ctx = static_cast<PE_WindowContextData*>(SDL_GetProperty(props, PE_OGL_CTX_KEY, nullptr));
		}
		else {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to get window context: %s"), SDL_GetError());
		}

		if (ctx) {
			PE_SetGLContextActiveFuncParams params = { nullptr, nullptr };
			ctx->worker.RunBlocking(PE_SetGLContextActiveFunc, &params);
			PE_DestroyGladContextFunc(&ctx->ctx);
			ctx->~PE_WindowContextData();
			PE_free(ctx);
		}
		else {
			PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Destroyed window with missing context property"));
		}
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
			PE_DestroyGladContextFunc(&primary_glad_ctx_);
		}
#endif

#if PE_OGL_BACKGROUND_LOADING
		if (background_glad_ctx_.userptr) {
			background_load_worker_.RunBlocking(PE_DestroyGladContextFunc, &background_glad_ctx_);
		}
#endif
	}

	SDL_Window* OpenGLGraphicsAdapter::CreateWindow(char* title, int width, int height, Uint32 flags) {
		flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
		PE_WindowParameters params{ this, title, width, height, flags };
		return static_cast<SDL_Window*>(PE_RunEventLoopFunctionBlocking(PE_CreateWindowFunc, &params));
	}

	void OpenGLGraphicsAdapter::DestroyWindow(SDL_Window* window) {
		if (window) {
			PE_RunEventLoopFunctionBlocking(PE_DestroyWindowFunc, window);
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
			SDL_GL_MakeCurrent(nullptr, nullptr);
			PE_SetGLContextActiveFuncParams set_params{ window, ctx };
			intptr_t set_active_ret = reinterpret_cast<intptr_t>(this_->background_load_worker_.RunBlocking(PE_SetGLContextActiveFunc, &set_params));
			if (set_active_ret != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set background OpenGL context active"));
				SDL_GL_DeleteContext(ctx);
				memset(&this_->background_glad_ctx_, 0, sizeof(GladGLContext));
				SDL_DestroyWindow(window);
				return nullptr;
			}

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
		{
			PE_WindowContextData* window_context = static_cast<PE_WindowContextData*>(PE_malloc(sizeof(PE_WindowContextData)));
			memset(window_context, 0, sizeof(PE_WindowContextData));
			new(window_context) PE_WindowContextData();
			if (window_context == nullptr) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to allocate window context"));
				SDL_DestroyWindow(window);
				return nullptr;
			}

			SDL_GLContext ctx = SDL_GL_CreateContext(window);
			if (ctx == nullptr) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create OpenGL context: %s"), SDL_GetError());
				SDL_DestroyWindow(window);
				window_context->~PE_WindowContextData();
				PE_free(window_context);
				return nullptr;
			}

			int ver = gladLoadGLContext(&window_context->ctx, SDL_GL_GetProcAddress);
			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Window GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
			SDL_GL_MakeCurrent(nullptr, nullptr);
			PE_SetGLContextActiveFuncParams set_params{ window, ctx };
			intptr_t set_active_ret = reinterpret_cast<intptr_t>(window_context->worker.RunBlocking(PE_SetGLContextActiveFunc, &set_params));
			if (set_active_ret != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set window OpenGL context active"));
				SDL_GL_DeleteContext(ctx);
				window_context->~PE_WindowContextData();
				PE_free(window_context);
				SDL_DestroyWindow(window);
				return nullptr;
			}

			window_context->window = window;
			window_context->ctx.userptr = ctx;
			SDL_PropertiesID window_props = SDL_GetWindowProperties(window);
			if (SDL_SetProperty(window_props, PE_OGL_CTX_KEY, window_context) != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set window context property: %s"), SDL_GetError());
				SDL_GL_DeleteContext(ctx);
				window_context->~PE_WindowContextData();
				PE_free(window_context);
				SDL_DestroyWindow(window);
				return nullptr;
			}
		}
#endif
		return window;
	}
}
#endif