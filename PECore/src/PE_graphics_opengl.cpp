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

		struct WindowContext {
			SDL_Window* window;
			GladGLContext glad_ctx;
			ThreadWorker worker;
		};

		OpenGlGraphicsImp* self = nullptr;

		// Helper PE_WorkFunction function that wraps other functions to allow passing parameters as a tuple
		template<typename FuncT, FuncT work, class ParamT>
		void WorkWrapper(void* userdata) {
			std::apply(work, *static_cast<ParamT*>(userdata));
		}

		// Helper function that runs functions on the OpenGL loading thread
		template<auto work, class... Args>
		PE_FORCEINLINE void RunLoadWork(Args&&... args) {
			auto params = std::forward_as_tuple(args ...);
#if PE_GL_BACKGROUND_LOADING
			self->background_worker.RunBlocking(WorkWrapper<decltype(work), work, decltype(params)>, static_cast<void*>(&params));
#else
			event_loop::RunWorkBlocking(WorkWrapper<decltype(work), work, decltype(params)>, static_cast<void*>(&params));
#endif
		}

		// Helper function that runs functions on the main/event thread
		template<auto work, class... Args>
		PE_FORCEINLINE void RunMainWork(Args&&... args) {
			auto params = std::forward_as_tuple(args ...);
			event_loop::RunWorkBlocking(WorkWrapper<decltype(work), work, decltype(params)>, static_cast<void*>(&params));
		}

		// Helper function that runs functions on the provided worker
		template<auto work, class... Args>
		PE_FORCEINLINE void RunWorker(ThreadWorker& worker, Args&&... args) {
			auto params = std::forward_as_tuple(args ...);
			worker.RunBlocking(WorkWrapper<decltype(work), work, decltype(params)>, static_cast<void*>(&params));
		}

		// return_vale is a SDL return value not PE_ERROR
		void SetContext(SDL_Window* window, SDL_GLContext ctx, int& return_val) {
			return_val = SDL_GL_MakeCurrent(window, ctx);
			if (return_val != 0) {
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

		void CreateSdlWindow(const char* title, int width, int height, Uint32 flags, SDL_Window*& return_window) {
			SDL_Window* window = SDL_CreateWindow(title, width, height, flags);
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
				int ctx_return;
				RunLoadWork<SetContext>(window, ctx, ctx_return);
				if (ctx_return != 0) {
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
				int ctx_return;
				RunWorker<SetContext>(window_context->worker, window, ctx, ctx_return);
				if (ctx_return != 0) {
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
			return_window = window;
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
				int ctx_return;
				RunWorker<SetContext>(window_ctx->worker, nullptr, nullptr, ctx_return);
				PE_DEBUG_ASSERT(ctx_return == 0, PE_TEXT("Failed to clear context"));
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

		void CreateGlProgram(const char* vertex_source, const char* frag_source, GlShader& shader) {
#if PE_GL_BACKGROUND_LOADING
			GladGLContext& gl = self->background_ctx;
#else
			GladGLContext& gl = self->primary_ctx;
#endif
			shader.program = 0;
			GLint compiled;
			GLuint vert_shader = gl.CreateShader(GL_VERTEX_SHADER);
			if (vert_shader == 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create vertex shader"));
				return;
			}

			gl.ShaderSource(vert_shader, 1, &vertex_source, nullptr);
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

			gl.ShaderSource(frag_shader, 1, &frag_source, nullptr);
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
					shader.program = program;
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
			SDL_Window* window = nullptr;
			flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
			RunMainWork<CreateSdlWindow>(title, width, height, flags, window);
			return window;
		}

		void DestroyWindowImp(SDL_Window* window) {
			if (window) {
				RunMainWork<DestroySdlWindow>(window);
			}
		}

		Shader* LoadShaderImp(const char* name) {
			PE_GL_REQUIRE_CTX(PE_TEXT("You must create a window before calling LoadShader"), nullptr);
			std::lock_guard cache_lock(self->shader_cache_mutex);
			const std::string* name_ptr = nullptr;
			try {
				auto cache_res = self->shader_cache.insert({ name, GlShader() });
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
				RunLoadWork<CreateGlProgram>(vertex_source.c_str(), fragment_source.c_str(), shader);
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
						RunLoadWork<DeleteGlProgram>(gl_shader);
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
#endif
