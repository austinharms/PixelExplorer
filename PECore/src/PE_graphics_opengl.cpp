#ifndef PE_DISABLE_OPENGL
#include "PE_graphics_opengl.h"
#include "PE_memory.h"
#include "PE_worker_thread.h"
#include "PE_errors.h"
#include "PE_log.h"
#include "PE_event_loop.h"
#include "PE_asset_manager_internal.h"
#include "SDL_video.h"
#include "glad/gl.h"
#include <new>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>

// Should all windows use one OpenGl context and render on the event loop aka main thread
// Note: this does not ensure only one context exists,
// if PE_OGL_BACKGROUND_LOADING is true it will still create it's own context
#ifndef PE_GL_SINGLE_CONTEXT
#define PE_GL_SINGLE_CONTEXT 0
#endif // !PE_GL_SINGLE_CONTEXT

// Should we create and use a worker thread and context to load OpenGL data
#ifndef PE_GL_BACKGROUND_LOADING
#define PE_GL_BACKGROUND_LOADING 1
#endif // !PE_GL_BACKGROUND_LOADING

#if PE_GL_SINGLE_CONTEXT == 0 && PE_GL_BACKGROUND_LOADING == 0
#error PE_GL_SINGLE_CONTEXT and or PE_GL_BACKGROUND_LOADING must be enabled
#endif

#define PE_GL_CTX_KEY PE_TEXT("PE_GL_CTX")

// Helper macro that runs functions on the OpenGL loading thread using RunLoadWork
#define PE_GL_LOAD(func, userdata) RunLoadWork<decltype(func), func, decltype(userdata)>(userdata);

// Helper macro that runs functions on the main/event thread using RunMainWork
#define PE_GL_MAIN(func, userdata) RunMainWork<decltype(func), func, decltype(userdata)>(userdata);

// Helper macro that runs functions on the worker using RunWorker
#define PE_GL_WORKER(worker, func, userdata) RunWorker<decltype(func), func, decltype(userdata)>(worker, userdata);
#if PE_GL_SINGLE_CONTEXT
#define PE_GL_REQUIRE_CTX(msg, rtn) if(self->primary_ctx.userptr == nullptr) { PE_LogError(PE_LOG_CATEGORY_RENDER, msg); return rtn; }
#else 
#define PE_GL_REQUIRE_CTX(msg, rtn) if(self->background_ctx.userptr == nullptr) { PE_LogError(PE_LOG_CATEGORY_RENDER, msg); return rtn; }
#endif

namespace pecore::graphics::implementation {
	namespace {
		struct GlShader : public Shader {
			size_t ref_count;
			const std::string* cache_key;
			GLuint program;
			bool loaded;

			GlShader() :
				ref_count(0),
				cache_key(nullptr),
				program(0),
				loaded(false) {
			}
		};

		struct OpenGlGraphicsImp : public GraphicsCommands {
#if PE_GL_SINGLE_CONTEXT
			GladGLContext primary_ctx;
			SDL_Window* active_window;
#endif
#if PE_GL_BACKGROUND_LOADING
			GladGLContext background_ctx;
			ThreadWorker background_worker;
#endif
			std::unordered_map<std::string, GlShader> shader_cache;
			std::mutex shader_cache_mutex;
			bool init;
		};

		struct CreateSdlWindowParams {
			const char* title;
			int width;
			int height;
			Uint32 flags;
			SDL_Window* return_window;
		};

		struct WindowContext {
			SDL_Window* window;
			GladGLContext glad_ctx;
			ThreadWorker worker;
		};

		struct SetContextParams {
			SDL_Window* window;
			SDL_GLContext ctx;
			// This is a SDL return value not PE_ERROR
			int return_val;
		};

		struct CreateGlProgramParams {
			const char* vertex_source;
			const char* frag_source;
			GlShader* shader;
		};

		OpenGlGraphicsImp* self = nullptr;

		// Helper function that wraps a work function and allow functions to have a "proper" parameter type
		template<PE_WorkFunction work, typename FuncT, class ParamT>
		PE_FORCEINLINE void WorkWrapper(void* userdata) {
			FuncT* work_real = reinterpret_cast<FuncT*>(work);
			work_real(static_cast<ParamT>(userdata));
		}

		// Helper function that runs functions on the OpenGL loading thread
		template<typename FuncT, FuncT work, class ParamT>
		PE_FORCEINLINE void RunLoadWork(ParamT userdata) {
#if PE_GL_BACKGROUND_LOADING
			self->background_worker.RunBlocking(WorkWrapper<reinterpret_cast<PE_WorkFunction>(work), FuncT, ParamT>, static_cast<void*>(userdata));
#else
			event_loop::RunWorkBlocking(WorkWrapper<reinterpret_cast<PE_WorkFunction>(work), FuncT, ParamT>, static_cast<void*>(userdata));
#endif
		}

		// Helper function that runs functions on the main/event thread
		template<typename FuncT, FuncT work, class ParamT>
		PE_FORCEINLINE void RunMainWork(ParamT userdata) {
			event_loop::RunWorkBlocking(WorkWrapper<reinterpret_cast<PE_WorkFunction>(work), FuncT, ParamT>, static_cast<void*>(userdata));
		}

		// Helper function that runs functions on the provided worker
		template<typename FuncT, FuncT work, class ParamT>
		PE_FORCEINLINE void RunWorker(ThreadWorker& worker, ParamT userdata) {
			worker.RunBlocking(WorkWrapper<reinterpret_cast<PE_WorkFunction>(work), FuncT, ParamT>, static_cast<void*>(userdata));
		}

		void SetContext(SetContextParams* params) {
			params->return_val = SDL_GL_MakeCurrent(params->window, params->ctx);
			if (params->return_val != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set OpenGL context active: %s"), SDL_GetError());
			}
		}

		// Destroys a GladGLContext and the corresponding SDL_GLContext
		// Note: it is expected that GladGLContext.userptr contains the SDL_GLContext
		void DestroyGladContext(GladGLContext* ctx) {
			if (SDL_GL_DeleteContext(ctx->userptr) != 0) {
				PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to delete OpenGL context: %s"), SDL_GetError());
			}

			memset(ctx, 0, sizeof(GladGLContext));
		}

		void CreateSdlWindow(CreateSdlWindowParams* params) {
			SDL_Window* window = SDL_CreateWindow(params->title, params->width, params->height, params->flags);
			if (!window) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window: %s"), SDL_GetError());
				return;
			}

#if PE_GL_BACKGROUND_LOADING
			if (self->background_ctx.userptr == nullptr) {
				SDL_GLContext ctx = SDL_GL_CreateContext(window);
				if (!ctx) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create background OpenGL context: %s"), SDL_GetError());
					SDL_DestroyWindow(window);
					return;
				}

				int ver = gladLoadGLContext(&self->background_ctx, SDL_GL_GetProcAddress);
				PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Background GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
				SDL_GL_MakeCurrent(nullptr, nullptr);
				SetContextParams set_params{ window, ctx, 0 };
				PE_GL_LOAD(SetContext, &set_params);
				if (set_params.return_val != 0) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set background OpenGL context active"));
					SDL_GL_DeleteContext(ctx);
					memset(&self->background_ctx, 0, sizeof(GladGLContext));
					SDL_DestroyWindow(window);
					return;
				}

				self->background_ctx.userptr = ctx;
			}
#endif
#if PE_GL_SINGLE_CONTEXT
			if (self->primary_ctx.userptr == nullptr) {
				self->active_window = window;
				SDL_GLContext ctx = SDL_GL_CreateContext(window);
				if (!ctx) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create OpenGL context: %s"), SDL_GetError());
					SDL_DestroyWindow(window);
					return;
				}

				int ver = gladLoadGLContext(&self->primary_ctx, SDL_GL_GetProcAddress);
				PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Primary GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
				self->primary_ctx.userptr = ctx;
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
				SetContextParams set_params{ window, ctx, 0 };
				PE_GL_WORKER(window_context->worker, SetContext, &set_params);
				if (set_params.return_val != 0) {
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
				if (SDL_SetProperty(window_props, PE_GL_CTX_KEY, window_context) != 0) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set window context property: %s"), SDL_GetError());
					window_context->~WindowContext();
					PE_free(window_context);
					SDL_GL_DeleteContext(ctx);
					SDL_DestroyWindow(window);
					return;
				}
			}
#endif
			params->return_window = window;
		}

		void DestroySdlWindow(SDL_Window* window) {
#if !PE_GL_SINGLE_CONTEXT
			WindowContext* window_ctx = nullptr;
			SDL_PropertiesID props = SDL_GetWindowProperties(window);
			if (props != 0) {
				window_ctx = static_cast<WindowContext*>(SDL_GetProperty(props, PE_GL_CTX_KEY, nullptr));
			}
			else {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to get window context: %s"), SDL_GetError());
			}

			if (window_ctx) {
				SetContextParams ctx_params{ nullptr, nullptr, 0 };
				PE_GL_WORKER(window_ctx->worker, SetContext, &ctx_params);
				DestroyGladContext(&window_ctx->glad_ctx);
				window_ctx->~WindowContext();
				PE_free(window_ctx);
			}
			else {
				PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Destroyed window with missing context property"));
			}
#endif
			SDL_DestroyWindow(static_cast<SDL_Window*>(window));
		}

		void CreateGlProgram(CreateGlProgramParams* params) {
#if PE_GL_BACKGROUND_LOADING
			GladGLContext& gl = self->background_ctx;
#else
			GladGLContext& gl = self->primary_ctx;
#endif
			params->shader->program = 0;
			GLint compiled;
			GLuint vert_shader = gl.CreateShader(GL_VERTEX_SHADER);
			if (vert_shader == 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create vertex shader"));
				return;
			}

			gl.ShaderSource(vert_shader, 1, &params->vertex_source, nullptr);
			gl.CompileShader(vert_shader);
			gl.GetShaderiv(vert_shader, GL_COMPILE_STATUS, &compiled);
			if (compiled != GL_TRUE)
			{
				GLchar message[256];
				gl.GetShaderInfoLog(vert_shader, 256, nullptr, message);
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to compile vertex shader, error: %s"), message);
				gl.DeleteProgram(vert_shader);
				return;
			}

			GLuint frag_shader = gl.CreateShader(GL_FRAGMENT_SHADER);
			if (frag_shader == 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create fragment shader"));
				gl.DeleteProgram(vert_shader);
				return;
			}

			gl.ShaderSource(frag_shader, 1, &params->frag_source, nullptr);
			gl.CompileShader(frag_shader);
			gl.GetShaderiv(frag_shader, GL_COMPILE_STATUS, &compiled);
			if (compiled != GL_TRUE)
			{
				GLchar message[256];
				gl.GetShaderInfoLog(frag_shader, 256, nullptr, message);
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to compile fragment shader, error: %s"), message);
				gl.DeleteProgram(vert_shader);
				gl.DeleteProgram(frag_shader);
				return;
			}

			GLuint program = gl.CreateProgram();
			if (program != 0) {
				gl.AttachShader(program, vert_shader);
				gl.AttachShader(program, frag_shader);
				gl.LinkProgram(program);
				gl.GetProgramiv(program, GL_LINK_STATUS, &compiled);
				if (compiled != GL_TRUE)
				{
					GLsizei log_length = 0;
					GLchar message[256];
					gl.GetProgramInfoLog(program, 1024, &log_length, message);
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to compile shader, error: %s"), message);
					gl.DeleteProgram(program);
				}
				else {
					params->shader->program = program;
				}
			}

			gl.DeleteProgram(vert_shader);
			gl.DeleteProgram(frag_shader);
		}

		void DeleteGlProgram(GlShader* shader) {
#if PE_GL_BACKGROUND_LOADING
			GladGLContext& gl = self->background_ctx;
#else
			GladGLContext& gl = self->primary_ctx;
#endif

			gl.DeleteProgram(shader->program);
			shader->program = 0;
		}

		// Create graphics *Imp function declaration
#define PE_GENERATED_GRAPHICS_API(rc, fn, params, args, ret) rc fn##Imp params;
#include "PE_generated_graphics_api.h"
#undef PE_GENERATED_GRAPHICS_API

		// Error Codes:
		//		 0: No Error
		//		-1: Failed to set SDL_GL_SHARE_WITH_CURRENT_CONTEXT Attribute
		//		-2: Failed to set SDL_GL_DOUBLEBUFFER Attribute
		//		-3: Failed to set SDL_GL_CONTEXT_MAJOR_VERSION Attribute
		//		-4: Failed to set SDL_GL_CONTEXT_MINOR_VERSION Attribute
		//		-5: Failed to set SDL_GL_CONTEXT_PROFILE_MASK Attribute
		int InitSystemImp() {
			// This should not be possible but we should return an error all the same
			if (self == nullptr) {
				return PE_ERROR_OUT_OF_MEMORY;
			}

			if (self->init) {
				PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Called Init on OpenGl implementation multiple times"));
				return PE_ERROR_ALREADY_INITIALIZED;
			}

			self = new(self) OpenGlGraphicsImp();
			self->init = true;

#if PE_GL_SINGLE_CONTEXT
			memset(&self->primary_ctx, 0, sizeof(GladGLContext));
			self->active_window = nullptr;
#endif
#if PE_GL_BACKGROUND_LOADING
			memset(&self->background_ctx, 0, sizeof(GladGLContext));
#endif
#if !PE_GL_SINGLE_CONTEXT || PE_GL_BACKGROUND_LOADING
			if (SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, SDL_TRUE) != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_SHARE_WITH_CURRENT_CONTEXT: %s"), SDL_GetError());
				return -1;
			}
#endif
			if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_DOUBLEBUFFER: %s"), SDL_GetError());
				return -2;
			}

			if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_CONTEXT_MAJOR_VERSION: %s"), SDL_GetError());
				return -3;
			}

			if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_CONTEXT_MINOR_VERSION: %s"), SDL_GetError());
				return -4;
			}

			if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_CONTEXT_PROFILE_MASK: %s"), SDL_GetError());
				return -5;
			}

			// Copy function pointers into GraphicsCommands struct
#define PE_GENERATED_GRAPHICS_API(rc, fn, params, args, ret) self->##fn = fn##Imp;
#include "PE_generated_graphics_api.h"
#undef PE_GENERATED_GRAPHICS_API
			return PE_ERROR_NONE;
		}

		void QuitSystemImp() {
			if (!self) {
				return;
			}

			if (self->init) {
				self->~OpenGlGraphicsImp();
			}

			PE_free(self);
			self = nullptr;
		}

		SDL_Window* CreateWindowImp(char* title, int width, int height, Uint32 flags) {
			flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
			CreateSdlWindowParams params{ title, width, height, flags, nullptr };
			PE_GL_MAIN(CreateSdlWindow, &params);
			return params.return_window;
		}

		void DestroyWindowImp(SDL_Window* window) {
			if (window) {
				PE_GL_MAIN(DestroySdlWindow, window);
			}
		}

		Shader* LoadShaderImp(const char* name) {
			PE_GL_REQUIRE_CTX(PE_TEXT("You must create a window before calling LoadShader"), nullptr);
			std::lock_guard cache_lock(self->shader_cache_mutex);
			const std::string* name_ptr = nullptr;
			try {
				auto cache_res = self->shader_cache.insert({ name, GlShader()});
				name_ptr = &cache_res.first->first;
				GlShader& shader = cache_res.first->second;
				if (shader.loaded) {
					shader.ref_count += 1;
					return static_cast<Shader*>(&shader);
				}

				std::filesystem::path shader_path(std::filesystem::path(PE_TEXT("res")) / PE_TEXT("shaders") / name);
				shader_path.concat(PE_TEXT(".pegls"));
				PE_LogVerbose(PE_LOG_CATEGORY_RENDER, PE_TEXT("Loading Shader %s from %s"), name, shader_path.generic_string().c_str());
				std::ifstream shader_stream;
				std::streampos shader_end;
				int err = PE_GetFileHandle(shader_path, shader_stream, shader_end);
				if (err != PE_ERROR_NONE) {
					throw err;
				}

				// 0: vertex source
				// 1: fragment source
				std::stringstream shader_source_streams[2];
				int cur_shader = -1;
				for (std::string line; shader_stream.tellg() < shader_end && std::getline(shader_stream, line);) {
					if (line.rfind("#shader vertex", 0) == 0) {
						cur_shader = 0;
					}
					else if (line.rfind("#shader fragment", 0) == 0) {
						cur_shader = 1;
					}
					else if (cur_shader < 0 || cur_shader > 1) {
						throw std::exception("failed to parse OpenGL shader file");
					}
					else {
						if (!line.empty() && line[line.size() - 1] == '\r') {
							line.erase(line.size() - 1);
						}

						shader_source_streams[cur_shader] << line << std::endl;
					}
				}

				std::string vertex_source(shader_source_streams[0].str());
				shader_source_streams[0].clear();
				std::string fragment_source(shader_source_streams[1].str());
				shader_source_streams[1].clear();
				CreateGlProgramParams create_params{ vertex_source.c_str(), fragment_source.c_str(), &shader };
				PE_GL_LOAD(CreateGlProgram, &create_params);
				if (shader.program == 0) {
					throw PE_ERROR_GENERAL;
				}

				shader.cache_key = name_ptr;
				shader.ref_count = 1;
				shader.loaded = true;
				return static_cast<Shader*>(&shader);
			}
			catch (const int& code) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader %s, error code: %") SDL_PRIs32, name, code);
			}
			catch (const std::exception& e) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader %s, error: %s"), name, e.what());
			}
			catch (...) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader %s, unknown error"), name);
			}

			if (name_ptr) {
				try {
					self->shader_cache.erase(*name_ptr);
				}
				catch (...) {}
			}

			return nullptr;
		}

		void UnloadShaderImp(Shader* shader) {
			if (shader) {
				GlShader* gl_shader = static_cast<GlShader*>(shader);
				if (--gl_shader->ref_count == 0) {
					std::lock_guard cache_lock(self->shader_cache_mutex);
					if (gl_shader->ref_count == 0) {
						PE_GL_LOAD(DeleteGlProgram, gl_shader);
						self->shader_cache.erase(*(gl_shader->cache_key));
					}
				}
			}
		}
	}

	PE_NODISCARD GraphicsCommands* PE_CALL AllocateOpenGlImplementation()
	{
		PE_DEBUG_ASSERT(self == nullptr, PE_TEXT("Only one OpenGL implementation can be created at a time"));
		self = static_cast<OpenGlGraphicsImp*>(PE_malloc(sizeof(OpenGlGraphicsImp)));
		if (!self) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate OpenGL implementation"));
			return nullptr;
		}

		memset(self, 0, sizeof(OpenGlGraphicsImp));
		self->InitSystem = InitSystemImp;
		self->QuitSystem = QuitSystemImp;
		return static_cast<GraphicsCommands*>(self);
	}
}

//#include "PE_opengl_adapter.h"
//#include "PE_event_loop.h"
//#include "PE_worker_thread.h"
//#include "PE_asset_manager_internal.h"
//#include "PE_log.h"
//#include "PE_memory.h"
//#include "PE_errors.h"
//#include "SDL_video.h"
//#include <filesystem>
//#include <fstream>
//#include <sstream>
//
//#if PE_OGL_BACKGROUND_LOADING
//#define PE_OGL_TASK_RUNNER background_load_worker_.RunBlocking
//#else
//#define PE_OGL_TASK_RUNNER PE_RunEventLoopFunctionBlocking
//#endif
//
//#define PE_OGL_CTX_KEY PE_TEXT("PE_GLCTX")
//#if PE_OGL_BACKGROUND_LOADING
//#define PE_OGL_LOAD_CTX background_glad_ctx_
//#elif PE_OGL_SINGLE_CONTEXT
//#define PE_OGL_LOAD_CTX primary_glad_ctx_
//#else
//#error PE_OGL_SINGLE_CONTEXT and or PE_OGL_BACKGROUND_LOADING must be enabled
//#endif
//
//#define PE_OGL_REQUIRE_CTX(msg, rtn) if(PE_OGL_LOAD_CTX.userptr == nullptr) { PE_LogError(PE_LOG_CATEGORY_RENDER, msg); return rtn; }
//
//namespace pecore::pe_graphics::open_gl {
//	namespace {
//		template<typename Func>
//		void LoadOGL() {
//
//		}
//
//		template<typename Func>
//		void RenderOGL() {
//
//		}
//
//		struct PE_OGLShader : PE_Shader {
//			GLuint program;
//		};
//
//		struct WindowContext {
//			SDL_Window* window;
//			GladGLContext glad_ctx;
//			ThreadWorker worker;
//		};
//
//		struct SetGLContextParams {
//			SDL_Window* window;
//			SDL_GLContext context;
//			// Note: this returns a SDL error value NOT PE_ERROR
//			int return_code;
//		};
//	}
//
//	struct OpenGLGraphicsAdapter::CreateWindowParams {
//		OpenGLGraphicsAdapter* self;
//		char* title;
//		int width;
//		int height;
//		Uint32 flags;
//		SDL_Window* return_window;
//	};
//
//	struct OpenGLGraphicsAdapter::CreateGLShaderParams {
//		GladGLContext* glad_ctx;
//		PE_OGLShader* shader;
//		const char* vertex_source;
//		const char* fragment_source;
//		bool created_shader;
//	};
//
//	// Error Codes:
//	//		 0: No Error
//	//		-1: Failed to set SDL_GL_SHARE_WITH_CURRENT_CONTEXT Attribute
//	//		-2: Failed to set SDL_GL_DOUBLEBUFFER Attribute
//	//		-3: Failed to set SDL_GL_CONTEXT_MAJOR_VERSION Attribute
//	//		-4: Failed to set SDL_GL_CONTEXT_MINOR_VERSION Attribute
//	//		-5: Failed to set SDL_GL_CONTEXT_PROFILE_MASK Attribute
//	OpenGLGraphicsAdapter::OpenGLGraphicsAdapter(int* retVal) : GraphicsAdapterInterface(retVal) {
//#if PE_OGL_SINGLE_CONTEXT
//		memset(&primary_glad_ctx_, 0, sizeof(GladGLContext));
//		active_ctx_window_ = nullptr;
//#endif
//#if PE_OGL_BACKGROUND_LOADING
//		memset(&background_glad_ctx_, 0, sizeof(GladGLContext));
//#endif
//#if !PE_OGL_SINGLE_CONTEXT || PE_OGL_BACKGROUND_LOADING
//		if (SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, SDL_TRUE) != 0) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_SHARE_WITH_CURRENT_CONTEXT: %s"), SDL_GetError());
//			*retVal = -1;
//		}
//#endif
//		if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_DOUBLEBUFFER: %s"), SDL_GetError());
//			*retVal = -2;
//		}
//
//		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_CONTEXT_MAJOR_VERSION: %s"), SDL_GetError());
//			*retVal = -3;
//		}
//
//		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_CONTEXT_MINOR_VERSION: %s"), SDL_GetError());
//			*retVal = -4;
//		}
//
//		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set attribute SDL_GL_CONTEXT_PROFILE_MASK: %s"), SDL_GetError());
//			*retVal = -5;
//		}
//
//		*retVal = 0;
//	}
//
//	OpenGLGraphicsAdapter::~OpenGLGraphicsAdapter() {
//#if PE_OGL_SINGLE_CONTEXT
//		if (primary_glad_ctx_.userptr) {
//			DestroyContextHelper(&primary_glad_ctx_);
//		}
//#endif
//
//#if PE_OGL_BACKGROUND_LOADING
//		if (background_glad_ctx_.userptr) {
//			background_load_worker_.RunBlocking(DestroyContextHelper, &background_glad_ctx_);
//		}
//#endif
//	}
//
//	SDL_Window* OpenGLGraphicsAdapter::CreateWindow(char* title, int width, int height, Uint32 flags) {
//		flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
//		CreateWindowParams params{ this, title, width, height, flags, nullptr };
//		PE_RunEventLoopFunctionBlocking(CreateWindowHelper, &params);
//		return static_cast<SDL_Window*>(params.return_window);
//	}
//
//	void OpenGLGraphicsAdapter::DestroyWindow(SDL_Window* window) {
//		if (window) {
//			PE_RunEventLoopFunctionBlocking(DestroyWindowHelper, window);
//		}
//	}
//
//	PE_Shader* OpenGLGraphicsAdapter::LoadShader(const char* name) {
//		PE_OGL_REQUIRE_CTX(PE_TEXT("You must create a window before calling LoadShader"), nullptr);
//		std::filesystem::path shader_path(std::filesystem::path(PE_TEXT("res")) / PE_TEXT("shaders") / name);
//		shader_path.concat(PE_TEXT(".pegls"));
//		PE_LogVerbose(PE_LOG_CATEGORY_RENDER, PE_TEXT("Loading Shader %s from %s"), name, shader_path.generic_string().c_str());
//		std::ifstream shader_stream;
//		std::streampos shader_end;
//		int err = PE_GetFileHandle(shader_path, shader_stream, shader_end);
//		if (err != PE_ERROR_NONE) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader %s, error code: %") SDL_PRIs32, name, err);
//			return nullptr;
//		}
//
//		PE_OGLShader* shader = static_cast<PE_OGLShader*>(PE_malloc(sizeof(PE_OGLShader)));
//		if (!shader) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate shader %s"), name);
//			return nullptr;
//		}
//
//		memset(shader, 0, sizeof(PE_OGLShader));
//		try {
//			// 0: vertex source
//			// 1: fragment source
//			std::stringstream shader_source_streams[2];
//			int cur_shader = -1;
//			for (std::string line; shader_stream.tellg() < shader_end && std::getline(shader_stream, line);) {
//				if (line.rfind("#shader vertex", 0) == 0) {
//					cur_shader = 0;
//				}
//				else if (line.rfind("#shader fragment", 0) == 0) {
//					cur_shader = 1;
//				}
//				else if (cur_shader < 0 || cur_shader > 1) {
//					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader %s, failed to parse OpenGL shader file"), name);
//					PE_free(shader);
//					return nullptr;
//				}
//				else {
//					if (!line.empty() && line[line.size() - 1] == '\r') {
//						line.erase(line.size() - 1);
//					}
//
//					shader_source_streams[cur_shader] << line << std::endl;
//				}
//			}
//
//			std::string vertex_source(shader_source_streams[0].str());
//			shader_source_streams[0].clear();
//			std::string fragment_source(shader_source_streams[1].str());
//			shader_source_streams[1].clear();
//			CreateGLShaderParams create_params{};
//			create_params.glad_ctx = &PE_OGL_LOAD_CTX;
//			create_params.created_shader = false;
//			create_params.shader = shader;
//			create_params.vertex_source = vertex_source.c_str();
//			create_params.fragment_source = fragment_source.c_str();
//			PE_OGL_TASK_RUNNER(CreateShaderHelper, &create_params);
//			if (!create_params.created_shader) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader %s"), name);
//				PE_free(shader);
//				return nullptr;
//			}
//		}
//		catch (const std::bad_alloc& e) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader %s, error: %s"), name, e.what());
//			PE_free(shader);
//			return nullptr;
//		}
//		catch (...) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader %s, unknow error"), name);
//			PE_free(shader);
//			return nullptr;
//		}
//
//		return static_cast<PE_Shader*>(shader);;
//	}
//
//	void OpenGLGraphicsAdapter::UnloadShader(PE_Shader* shader) {
//		if (shader != nullptr) {
//
//		}
//	}
//
//	void OpenGLGraphicsAdapter::CreateWindowHelper(void* window_parameters) {
//		CreateWindowParams* params = static_cast<CreateWindowParams*>(window_parameters);
//		params->self->CreateWindowHelperInternal(*params);
//	}
//
//	void OpenGLGraphicsAdapter::CreateWindowHelperInternal(CreateWindowParams& params) {
//		params.return_window = nullptr;
//		SDL_Window* window = SDL_CreateWindow(params.title, params.width, params.height, params.flags);
//		if (!window) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window: %s"), SDL_GetError());
//			return;
//		}
//
//#if PE_OGL_BACKGROUND_LOADING
//		if (background_glad_ctx_.userptr == nullptr) {
//			SDL_GLContext ctx = SDL_GL_CreateContext(window);
//			if (!ctx) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create background OpenGL context: %s"), SDL_GetError());
//				SDL_DestroyWindow(window);
//				return;
//			}
//
//			int ver = gladLoadGLContext(&background_glad_ctx_, SDL_GL_GetProcAddress);
//			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Background GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
//			SDL_GL_MakeCurrent(nullptr, nullptr);
//			SetGLContextParams set_params{ window, ctx, 0 };
//			background_load_worker_.RunBlocking(SetGLContextHelper, &set_params);
//			if (set_params.return_code != 0) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set background OpenGL context active"));
//				SDL_GL_DeleteContext(ctx);
//				memset(&background_glad_ctx_, 0, sizeof(GladGLContext));
//				SDL_DestroyWindow(window);
//				return;
//			}
//
//			background_glad_ctx_.userptr = ctx;
//		}
//#endif
//
//#if PE_OGL_SINGLE_CONTEXT
//		if (primary_glad_ctx_.userptr == nullptr) {
//			active_ctx_window_ = window;
//			SDL_GLContext ctx = SDL_GL_CreateContext(window);
//			if (!ctx) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create OpenGL context: %s"), SDL_GetError());
//				SDL_DestroyWindow(window);
//				return;
//			}
//
//			int ver = gladLoadGLContext(&primary_glad_ctx_, SDL_GL_GetProcAddress);
//			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Primary GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
//			primary_glad_ctx_.userptr = ctx;
//		}
//#else
//		{
//			WindowContext* window_context = static_cast<WindowContext*>(PE_malloc(sizeof(WindowContext)));
//			memset(window_context, 0, sizeof(WindowContext));
//			new(window_context) WindowContext();
//			if (window_context == nullptr) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to allocate window context"));
//				SDL_DestroyWindow(window);
//				return;
//			}
//
//			SDL_GLContext ctx = SDL_GL_CreateContext(window);
//			if (ctx == nullptr) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, failed to create OpenGL context: %s"), SDL_GetError());
//				SDL_DestroyWindow(window);
//				window_context->~WindowContext();
//				PE_free(window_context);
//				return;
//			}
//
//			int ver = gladLoadGLContext(&window_context->glad_ctx, SDL_GL_GetProcAddress);
//			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Window GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
//			SDL_GL_MakeCurrent(nullptr, nullptr);
//			SetGLContextParams set_params{ window, ctx, 0 };
//			window_context->worker.RunBlocking(SetGLContextHelper, &set_params);
//			if (set_params.return_code != 0) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set window OpenGL context active"));
//				SDL_GL_DeleteContext(ctx);
//				window_context->~WindowContext();
//				PE_free(window_context);
//				SDL_DestroyWindow(window);
//				return;
//			}
//
//			window_context->window = window;
//			window_context->glad_ctx.userptr = ctx;
//			SDL_PropertiesID window_props = SDL_GetWindowProperties(window);
//			if (SDL_SetProperty(window_props, PE_OGL_CTX_KEY, window_context) != 0) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set window context property: %s"), SDL_GetError());
//				SDL_GL_DeleteContext(ctx);
//				window_context->~WindowContext();
//				PE_free(window_context);
//				SDL_DestroyWindow(window);
//				return;
//			}
//		}
//#endif
//		params.return_window = window;
//	}
//
//	void OpenGLGraphicsAdapter::DestroyWindowHelper(void* window) {
//#if !PE_OGL_SINGLE_CONTEXT
//		WindowContext* window_ctx = nullptr;
//		SDL_PropertiesID props = SDL_GetWindowProperties(static_cast<SDL_Window*>(window));
//		if (props != 0) {
//			window_ctx = static_cast<WindowContext*>(SDL_GetProperty(props, PE_OGL_CTX_KEY, nullptr));
//		}
//		else {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to get window context: %s"), SDL_GetError());
//		}
//
//		if (window_ctx) {
//			SetGLContextParams params = { nullptr, nullptr };
//			window_ctx->worker.RunBlocking(SetGLContextHelper, &params);
//			DestroyContextHelper(&window_ctx->glad_ctx);
//			window_ctx->~WindowContext();
//			PE_free(window_ctx);
//		}
//		else {
//			PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Destroyed window with missing context property"));
//		}
//#endif
//
//		SDL_DestroyWindow(static_cast<SDL_Window*>(window));
//	}
//
//	void OpenGLGraphicsAdapter::SetGLContextHelper(void* context_parameters) {
//		SetGLContextParams* params = static_cast<SetGLContextParams*>(context_parameters);
//		params->return_code = SDL_GL_MakeCurrent(params->window, params->context);
//		if (params->return_code != 0) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set OpenGL context active: %s"), SDL_GetError());
//		}
//	}
//
//	void OpenGLGraphicsAdapter::DestroyContextHelper(void* ctx) {
//		if (SDL_GL_DeleteContext(static_cast<GladGLContext*>(ctx)->userptr) != 0) {
//			PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to delete OpenGL context: %s"), SDL_GetError());
//		}
//
//		memset(ctx, 0, sizeof(GladGLContext));
//	}
//
//	void OpenGLGraphicsAdapter::CreateShaderHelper(void* shader_source)
//	{
//		CreateGLShaderParams* create_params = static_cast<CreateGLShaderParams*>(shader_source);
//		GladGLContext& ctx = *create_params->glad_ctx;
//		create_params->created_shader = false;
//		GLint compiled;
//		GLuint vert_shader = ctx.CreateShader(GL_VERTEX_SHADER);
//		if (vert_shader == 0) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create vertex shader"));
//			return;
//		}
//
//		ctx.ShaderSource(vert_shader, 1, &create_params->vertex_source, nullptr);
//		ctx.CompileShader(vert_shader);
//		ctx.GetShaderiv(vert_shader, GL_COMPILE_STATUS, &compiled);
//		if (compiled != GL_TRUE)
//		{
//			GLchar message[256];
//			ctx.GetShaderInfoLog(vert_shader, 256, nullptr, message);
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to compile vertex shader, error: %s"), message);
//			ctx.DeleteProgram(vert_shader);
//			return;
//		}
//
//		GLuint frag_shader = ctx.CreateShader(GL_FRAGMENT_SHADER);
//		if (frag_shader == 0) {
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create fragment shader"));
//			ctx.DeleteProgram(vert_shader);
//			return;
//		}
//
//		ctx.ShaderSource(frag_shader, 1, &create_params->fragment_source, nullptr);
//		ctx.CompileShader(frag_shader);
//		ctx.GetShaderiv(frag_shader, GL_COMPILE_STATUS, &compiled);
//		if (compiled != GL_TRUE)
//		{
//			GLchar message[256];
//			ctx.GetShaderInfoLog(frag_shader, 256, nullptr, message);
//			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to compile fragment shader, error: %s"), message);
//			ctx.DeleteProgram(vert_shader);
//			ctx.DeleteProgram(frag_shader);
//			return;
//		}
//
//		GLuint program = ctx.CreateProgram();
//		if (program != 0) {
//			ctx.AttachShader(program, vert_shader);
//			ctx.AttachShader(program, frag_shader);
//			ctx.LinkProgram(program);
//			ctx.GetProgramiv(program, GL_LINK_STATUS, &compiled);
//			if (compiled != GL_TRUE)
//			{
//				GLsizei log_length = 0;
//				GLchar message[256];
//				ctx.GetProgramInfoLog(program, 1024, &log_length, message);
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to compile shader, error: %s"), message);
//				ctx.DeleteProgram(program);
//			}
//			else {
//				create_params->shader->program = program;
//				create_params->created_shader = true;
//			}
//		}
//
//		ctx.DeleteProgram(vert_shader);
//		ctx.DeleteProgram(frag_shader);
//	}
//}
//
//#undef PE_OGL_TASK_RUNNER
//#undef PE_OGL_REQUIRE_CTX
#endif
