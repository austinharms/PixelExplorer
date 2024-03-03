#ifndef PE_DISABLE_OPENGL
#include "PE_opengl_adapter.h"
#include "PE_event_loop.h"
#include "PE_worker_thread.h"
#include "PE_asset_manager_internal.h"
#include "PE_log.h"
#include "PE_memory.h"
#include "PE_errors.h"
#include "SDL_video.h"
#include <filesystem>
#include <fstream>

#define PE_OGL_CTX_KEY PE_TEXT("PE_GLCTX")
#if PE_OGL_BACKGROUND_LOADING
#define PE_OGL_TASK_RUNNER background_load_worker_.RunBlocking
#else
#define PE_OGL_TASK_RUNNER PE_RunEventLoopFunctionBlocking
#endif

namespace pecore::pe_graphics::open_gl {
	namespace {
		struct PE_OGLShader : PE_Shader {
			GLuint vertex_shader;
			GLuint fragment_shader;
		};

		struct WindowContext {
			SDL_Window* window;
			GladGLContext glad_ctx;
			ThreadWorker worker;
		};

		struct SetGLContextParams {
			SDL_Window* window;
			SDL_GLContext context;
			// Note: this returns a SDL error value NOT PE_ERROR
			int return_code;
		};
	}

	struct OpenGLGraphicsAdapter::CreateWindowParams {
		OpenGLGraphicsAdapter* self;
		char* title;
		int width;
		int height;
		Uint32 flags;
		SDL_Window* return_window;
	};

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
			DestroyContextHelper(&primary_glad_ctx_);
		}
#endif

#if PE_OGL_BACKGROUND_LOADING
		if (background_glad_ctx_.userptr) {
			background_load_worker_.RunBlocking(DestroyContextHelper, &background_glad_ctx_);
		}
#endif
	}

	SDL_Window* OpenGLGraphicsAdapter::CreateWindow(char* title, int width, int height, Uint32 flags) {
		flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
		CreateWindowParams params{ this, title, width, height, flags, nullptr };
		PE_RunEventLoopFunctionBlocking(CreateWindowHelper, &params);
		return static_cast<SDL_Window*>(params.return_window);
	}

	void OpenGLGraphicsAdapter::DestroyWindow(SDL_Window* window) {
		if (window) {
			PE_RunEventLoopFunctionBlocking(DestroyWindowHelper, window);
		}
	}

	PE_Shader* OpenGLGraphicsAdapter::LoadShader(const char* name) {
		//std::filesystem::path shader_path(std::filesystem::path(PE_TEXT("res")) / PE_TEXT("shaders") / name);
		//shader_path.append(PE_TEXT(".pegls"));
		//std::ifstream shader_stream;
		//size_t shader_size;
		//int err = PE_GetFileHandle(shader_path, shader_stream, shader_size);
		//if (err != PE_ERROR_NONE) {
		//	PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader %s, error code: %") SDL_PRIs32, name, err);
		//	return nullptr;
		//}

		//PE_OGLShader* shader = static_cast<PE_OGLShader*>(PE_malloc(sizeof(PE_OGLShader)));
		//if (!shader) {
		//	PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate shader %s"), name);
		//	return nullptr;
		//}

		//memset(shader, 0, sizeof(PE_OGLShader));
		//PE_OGL_TASK_RUNNER();

		//shader->vertex_shader = ;

		//	if (!PE_ParseShaderFile(shader_stream, shader_size, *shader)) {
		//		PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader %s, failed to parse shader file"), name);
		//		return nullptr;
		//	}



		//return static_cast<PE_Shader*>(shader);
		return nullptr;
	}

	void OpenGLGraphicsAdapter::UnloadShader(PE_Shader* shader) {

	}

	void OpenGLGraphicsAdapter::CreateWindowHelper(void* window_parameters) {
		CreateWindowParams* params = static_cast<CreateWindowParams*>(window_parameters);
		params->self->CreateWindowHelperInternal(*params);
	}

	void OpenGLGraphicsAdapter::CreateWindowHelperInternal(CreateWindowParams& params) {
		params.return_window = nullptr;
		SDL_Window* window = SDL_CreateWindow(params.title, params.width, params.height, params.flags);
		if (!window) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window: %s"), SDL_GetError());
			return;
		}

#if PE_OGL_BACKGROUND_LOADING
		if (background_glad_ctx_.userptr == nullptr) {
			SDL_GLContext ctx = SDL_GL_CreateContext(window);
			if (!ctx) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create background OpenGL context: %s"), SDL_GetError());
				SDL_DestroyWindow(window);
				return;
			}

			int ver = gladLoadGLContext(&background_glad_ctx_, SDL_GL_GetProcAddress);
			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Background GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
			SDL_GL_MakeCurrent(nullptr, nullptr);
			SetGLContextParams set_params{ window, ctx, 0 };
			background_load_worker_.RunBlocking(SetGLContextHelper, &set_params);
			if (set_params.return_code != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set background OpenGL context active"));
				SDL_GL_DeleteContext(ctx);
				memset(&background_glad_ctx_, 0, sizeof(GladGLContext));
				SDL_DestroyWindow(window);
				return;
			}

			background_glad_ctx_.userptr = ctx;
		}
#endif

#if PE_OGL_SINGLE_CONTEXT
		if (primary_glad_ctx_.userptr == nullptr) {
			active_ctx_window_ = window;
			SDL_GLContext ctx = SDL_GL_CreateContext(window);
			if (!ctx) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create OpenGL context: %s"), SDL_GetError());
				SDL_DestroyWindow(window);
				return;
			}

			int ver = gladLoadGLContext(&primary_glad_ctx_, SDL_GL_GetProcAddress);
			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Primary GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
			primary_glad_ctx_.userptr = ctx;
		}
#else
		{
			WindowContext* window_context = static_cast<WindowContext*>(PE_malloc(sizeof(WindowContext)));
			memset(window_context, 0, sizeof(WindowContext));
			new(window_context) WindowContext();
			if (window_context == nullptr) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to allocate window context"));
				SDL_DestroyWindow(window);
				return;
			}

			SDL_GLContext ctx = SDL_GL_CreateContext(window);
			if (ctx == nullptr) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create OpenGL context: %s"), SDL_GetError());
				SDL_DestroyWindow(window);
				window_context->~WindowContext();
				PE_free(window_context);
				return;
			}

			int ver = gladLoadGLContext(&window_context->glad_ctx, SDL_GL_GetProcAddress);
			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Window GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
			SDL_GL_MakeCurrent(nullptr, nullptr);
			SetGLContextParams set_params{ window, ctx, 0 };
			window_context->worker.RunBlocking(SetGLContextHelper, &set_params);
			if (set_params.return_code != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set window OpenGL context active"));
				SDL_GL_DeleteContext(ctx);
				window_context->~WindowContext();
				PE_free(window_context);
				SDL_DestroyWindow(window);
				return;
			}

			window_context->window = window;
			window_context->glad_ctx.userptr = ctx;
			SDL_PropertiesID window_props = SDL_GetWindowProperties(window);
			if (SDL_SetProperty(window_props, PE_OGL_CTX_KEY, window_context) != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set window context property: %s"), SDL_GetError());
				SDL_GL_DeleteContext(ctx);
				window_context->~WindowContext();
				PE_free(window_context);
				SDL_DestroyWindow(window);
				return;
			}
		}
#endif
		params.return_window = window;
	}

	void OpenGLGraphicsAdapter::DestroyWindowHelper(void* window) {
#if !PE_OGL_SINGLE_CONTEXT
		WindowContext* window_ctx = nullptr;
		SDL_PropertiesID props = SDL_GetWindowProperties(static_cast<SDL_Window*>(window));
		if (props != 0) {
			window_ctx = static_cast<WindowContext*>(SDL_GetProperty(props, PE_OGL_CTX_KEY, nullptr));
		}
		else {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to get window context: %s"), SDL_GetError());
		}

		if (window_ctx) {
			SetGLContextParams params = { nullptr, nullptr };
			window_ctx->worker.RunBlocking(SetGLContextHelper, &params);
			DestroyContextHelper(&window_ctx->glad_ctx);
			window_ctx->~WindowContext();
			PE_free(window_ctx);
		}
		else {
			PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Destroyed window with missing context property"));
		}
#endif

		SDL_DestroyWindow(static_cast<SDL_Window*>(window));
	}

	void OpenGLGraphicsAdapter::SetGLContextHelper(void* context_parameters) {
		SetGLContextParams* params = static_cast<SetGLContextParams*>(context_parameters);
		params->return_code = SDL_GL_MakeCurrent(params->window, params->context);
		if (params->return_code != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set OpenGL context active: %s"), SDL_GetError());
		}
	}

	void OpenGLGraphicsAdapter::DestroyContextHelper(void* ctx) {
		if (SDL_GL_DeleteContext(static_cast<GladGLContext*>(ctx)->userptr) != 0) {
			PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to delete OpenGL context: %s"), SDL_GetError());
		}

		memset(ctx, 0, sizeof(GladGLContext));
	}
}

#undef PE_OGL_TASK_RUNNER
#endif