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

// Returns load thread worker or PE_EventLoop if there is no load worker
#if PE_GL_BACKGROUND_LOADING
#define PE_GL_WORKER this_->load_thread_worker
#else
#define PE_GL_WORKER PE_EventLoop
#endif

//#define PE_GL_CTX_KEY PE_TEXT("PE_GL_CTX")
//
//#if PE_GL_SINGLE_CONTEXT
//#define PE_GL_REQUIRE_CTX(msg, rtn) if(self->primary_ctx.userptr == nullptr) { PE_LogError(PE_LOG_CATEGORY_RENDER, msg); return rtn; }
//#else 
//#define PE_GL_REQUIRE_CTX(msg, rtn) if(self->background_ctx.userptr == nullptr) { PE_LogError(PE_LOG_CATEGORY_RENDER, msg); return rtn; }
//#endif

#define PE_GL_REQUIRE_CTX(msg, rtn)

namespace pecore::graphics::implementation {
	namespace {
		constexpr GLenum GlIndexTypeLookup[] = { GL_UNSIGNED_INT, GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };
		PE_STATIC_ASSERT(PE_ARRAY_LEN(GlIndexTypeLookup) == PE_INDEX_ENUM_VALUE_COUNT, PE_TEXT("GlIndexTypeLookup array need to be updated"));
		constexpr GLenum GlMeshAttribTypeLookup[] = { GL_FLOAT, GL_HALF_FLOAT, GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT };
		PE_STATIC_ASSERT(PE_ARRAY_LEN(GlMeshAttribTypeLookup) == PE_MESH_ATR_ENUM_VALUE_COUNT, PE_TEXT("GlMeshAttribTypeLookup array need to be updated"));
		constexpr GLenum GlCullFaceLookup[] = { GL_BACK, GL_FRONT, GL_FRONT_AND_BACK, GL_BACK };
		PE_STATIC_ASSERT(PE_ARRAY_LEN(GlCullFaceLookup) == PE_CULL_ENUM_VALUE_COUNT, PE_TEXT("GlCullFaceLookup array need to be updated"));

		struct PE_Shader {
			size_t ref_count;
			const std::string* cache_key;
			GLuint program;
			bool loaded;

			PE_Shader() :
				ref_count(0),
				cache_key(nullptr),
				program(0),
				loaded(false) {
			}
		};

		//struct RenderMeshAttrib {
		//	GLenum type;
		//	int32_t location;
		//	int32_t size;
		//	GLboolean normalized;
		//	int32_t stride;
		//	int32_t offset;
		//};

		struct PE_RenderMesh {
			enum {
				BUF_VERTEX = 0,
				BUF_ELEMENT = 1,
			};

			GLuint buffers[2];
		};

		enum CommandType : uint32_t
		{
			GL_CMD_SHADER = 0,
			GL_CMD_MESH_FORMAT,
			GL_CMD_MESH,
			GL_CMD_DRAW,
			GL_CMD_UNIFORM_VALUE,
			GL_CMD_CULL_FACE,
		};

		template<class ValueT>
		struct CmdPad {
			enum { val = sizeof(ValueT) % 8 };
		};

		template<class ValueT, size_t PAD>
		struct CommandBase {
			constexpr CommandBase(CommandType cmd_type, const ValueT& cmd_value) :
				cmd_size(sizeof(ValueT) + sizeof(uint32_t) + sizeof(CommandType) + PAD),
				type(cmd_type),
				value(cmd_value) {
			}

			uint32_t cmd_size;
			CommandType type;
			ValueT value;
			uint8_t pad[PAD];
		};

		template<class ValueT>
		struct CommandBase<ValueT, 0> {
			constexpr CommandBase(CommandType cmd_type, const ValueT& cmd_value) :
				cmd_size(sizeof(ValueT) + sizeof(uint32_t) + sizeof(CommandType)),
				type(cmd_type),
				value(cmd_value) {
			}

			uint32_t cmd_size;
			CommandType type;
			ValueT value;
		};

		template<class ValueT>
		struct Command : public CommandBase<ValueT, CmdPad<ValueT>::val> {
			using CommandBase<ValueT, CmdPad<ValueT>::val>::CommandBase;
		};

		struct CommandBuffer {
			CommandBuffer* next;
			uint64_t* tail;
			uint64_t data[2048];
		};

		struct CommandQueue {
			CommandBuffer command_buffer;
			CommandBuffer* active_buffer;
		};

		struct CommandExecutionState {
			GLuint program;
			GLuint mesh_vao;
			GLenum index_type;
		};

		struct Window : public PE_Window {
#if !PE_GL_SINGLE_CONTEXT
			GladGLContext glad_render_context;
			SDL_GLContext opengl_render_context;
			ThreadWorker render_thread_worker;
			GLuint opengl_render_vao;
#endif
		};

		struct OpenGlGraphicsCommands : public GraphicsCommands {
#if PE_GL_SINGLE_CONTEXT
			GladGLContext glad_render_context;
			SDL_GLContext opengl_render_context;
			SDL_Window* active_render_window;
			GLuint opengl_render_vao;
#endif
#if PE_GL_BACKGROUND_LOADING
			GladGLContext glad_load_context;
			SDL_GLContext opengl_load_context;
			ThreadWorker load_thread_worker;
#endif
			std::unordered_map<std::string, PE_Shader> shader_cache;
			std::mutex shader_cache_mutex;
			bool init;
		};

		OpenGlGraphicsCommands* this_ = nullptr;

		// Returns glad load context or glad render context if there is no load context
		PE_NODISCARD PE_FORCEINLINE GladGLContext& GetGladContext() {
#if PE_GL_BACKGROUND_LOADING
			return this_->glad_load_context;
#else
			return this_->glad_render_context;
#endif
		}

		// return_vale is a SDL return value not PE_ERROR
		void SetOpenGlContext(SDL_Window* window, SDL_GLContext ctx, int& return_val) {
			return_val = SDL_GL_MakeCurrent(window, ctx);
			if (return_val != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to set OpenGL context active: %s"), SDL_GetError());
			}
		}

#if PE_GL_BACKGROUND_LOADING && !PE_GL_SINGLE_CONTEXT
		// Helper that creates a gl context on the load thread
		// This is required as SDL creates share lists with the active context and the only context we can assume exists is the load one
		void CreateWindowGlContextHelper(Window& window, int& return_code) {
			PE_DEBUG_ASSERT(!this_->opengl_load_context, PE_TEXT("Attempted to create Window OpenGl context without background OpenGl context"));
			window.opengl_render_context = SDL_GL_CreateContext(window.window_handle);
			if (!window.opengl_render_context) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window OpenGL context: %s"), SDL_GetError());
				// TODO if creating a context fails can we assume the old context is still active?
				SetOpenGlContext(window.window_handle, this_->opengl_load_context, return_code);
				// We can not recover without a load context, abort
				PE_ASSERT(return_code == PE_ERROR_NONE, PE_TEXT("Failed to reactivate OpenGl load context"));
				return_code = PE_ERROR_GENERAL;
				return;
			}

			int ver = gladLoadGLContext(&window.glad_render_context, SDL_GL_GetProcAddress);
			PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Render GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
			SetOpenGlContext(window.window_handle, this_->opengl_load_context, return_code);
			// We can not recover without a load context, abort
			PE_ASSERT(return_code == PE_ERROR_NONE, PE_TEXT("Failed to reactivate OpenGl load context"));
			window.render_thread_worker.PushBlockingTWork<SetOpenGlContext>(window.window_handle, window.opengl_render_context, return_code);
			if (return_code != PE_ERROR_NONE) {
				SDL_GL_DeleteContext(window.opengl_render_context);
			}
		}
#endif

		int CreateWindowGlContext(Window& window) {
#if PE_GL_BACKGROUND_LOADING && PE_GL_SINGLE_CONTEXT
			if (!this_->opengl_render_context) {
				PE_DEBUG_ASSERT(!this_->opengl_load_context, PE_TEXT("Primary OpenGl context without background OpenGl context"));
				SDL_GLContext bg_ctx = SDL_GL_CreateContext(window.window_handle);
				if (!bg_ctx) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create background OpenGL context: %s"), SDL_GetError());
					return PE_ERROR_GENERAL;
				}

				int ver = gladLoadGLContext(&this_->glad_load_context, SDL_GL_GetProcAddress);
				PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Background GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));

				SDL_GLContext main_ctx = SDL_GL_CreateContext(window.window_handle);
				if (!main_ctx) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create primary OpenGL context: %s"), SDL_GetError());
					SDL_GL_DeleteContext(bg_ctx);
					return PE_ERROR_GENERAL;
				}

				ver = gladLoadGLContext(&this_->glad_render_context, SDL_GL_GetProcAddress);
				PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Primary GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
				int set_rtn;
				this_->load_thread_worker.PushBlockingTWork<SetOpenGlContext>(window.window_handle, bg_ctx, set_rtn);
				if (set_rtn != 0) {
					SDL_GL_DeleteContext(main_ctx);
					SDL_GL_DeleteContext(bg_ctx);
					return PE_ERROR_GENERAL;
				}

				this_->glad_render_context.CreateVertexArrays(1, &this_->opengl_render_vao);
				if (this_->opengl_render_vao == 0) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create primary OpenGL vertex array object"));
					SDL_GL_DeleteContext(main_ctx);
					SDL_GL_DeleteContext(bg_ctx);
					return PE_ERROR_GENERAL;
				}

				this_->opengl_load_context = bg_ctx;
				this_->opengl_render_context = main_ctx;
			}
#elif PE_GL_SINGLE_CONTEXT
			if (!this_->opengl_render_context) {
				SDL_GLContext main_ctx = SDL_GL_CreateContext(window.window_handle);
				if (!main_ctx) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create primary OpenGL context: %s"), SDL_GetError());
					return PE_ERROR_GENERAL;
				}

				int ver = gladLoadGLContext(&this_->glad_render_context, SDL_GL_GetProcAddress);
				PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Primary GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
				this_->glad_render_context.CreateVertexArrays(1, &this_->opengl_render_vao);
				if (this_->opengl_render_vao == 0) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create primary OpenGL vertex array object"));
					SDL_GL_DeleteContext(main_ctx);
					return PE_ERROR_GENERAL;
				}

				this_->opengl_render_context = main_ctx;
			}
#elif PE_GL_BACKGROUND_LOADING && !PE_GL_SINGLE_CONTEXT
			if (!this_->opengl_load_context) {
				SDL_GLContext bg_ctx = SDL_GL_CreateContext(window.window_handle);
				if (!bg_ctx) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create background OpenGL context: %s"), SDL_GetError());
					return PE_ERROR_GENERAL;
				}

				int ver = gladLoadGLContext(&this_->glad_load_context, SDL_GL_GetProcAddress);
				PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Background GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));

				window.opengl_render_context = SDL_GL_CreateContext(window.window_handle);
				if (!window.opengl_render_context) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window OpenGL context: %s"), SDL_GetError());
					SDL_GL_DeleteContext(bg_ctx);
					return PE_ERROR_GENERAL;
				}

				ver = gladLoadGLContext(&window.glad_render_context, SDL_GL_GetProcAddress);
				PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Render GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(ver), GLAD_VERSION_MINOR(ver));
				window.glad_render_context.CreateVertexArrays(1, &window.opengl_render_vao);
				if (window.opengl_render_vao == 0) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create render OpenGL vertex array object"));
					SDL_GL_DeleteContext(window.opengl_render_context);
					SDL_GL_DeleteContext(bg_ctx);
					return PE_ERROR_GENERAL;
				}

				int set_rtn = SDL_GL_MakeCurrent(nullptr, nullptr);
				if (set_rtn != 0) {
					SDL_GL_DeleteContext(window.opengl_render_context);
					SDL_GL_DeleteContext(bg_ctx);
					return PE_ERROR_GENERAL;
				}

				window.render_thread_worker.PushBlockingTWork<SetOpenGlContext>(window.window_handle, window.opengl_render_context, set_rtn);
				if (set_rtn != 0) {
					SDL_GL_DeleteContext(window.opengl_render_context);
					SDL_GL_DeleteContext(bg_ctx);
					return PE_ERROR_GENERAL;
				}

				this_->load_thread_worker.PushBlockingTWork<SetOpenGlContext>(window.window_handle, bg_ctx, set_rtn);
				if (set_rtn != 0) {
					SDL_GL_DeleteContext(window.opengl_render_context);
					SDL_GL_DeleteContext(bg_ctx);
					return PE_ERROR_GENERAL;
				}

				this_->opengl_load_context = bg_ctx;
			}
			else {
				int rtn_code;
				this_->load_thread_worker.PushBlockingTWork<CreateWindowGlContextHelper>(window, rtn_code);
				return rtn_code;
			}
#else
#error INVALID PE_GL_* DEFINES
#endif
			return PE_ERROR_NONE;
		}

		void CreateWindow(const char* title, int width, int height, Uint32 flags, Window& window) {
			window.window_handle = SDL_CreateWindow(title, width, height, flags);
			if (!window.window_handle) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window: %s"), SDL_GetError());
				return;
			}

			if (int ret = CreateWindowGlContext(window) != PE_ERROR_NONE) {
				SDL_DestroyWindow(window.window_handle);
				window.window_handle = nullptr;
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window: %") SDL_PRIs32, ret);
				return;
			}
		}

		void DestroyWindow(Window* window) {
			if (!window) {
				return;
			}

#if !PE_GL_SINGLE_CONTEXT
			int rtn_code;
			window->render_thread_worker.PushBlockingTWork<SetOpenGlContext>(nullptr, nullptr, rtn_code);
			PE_DEBUG_ASSERT(rtn_code == PE_ERROR_NONE, PE_TEXT("Failed to clear window render context"));
			SDL_GL_DeleteContext(window->opengl_render_context);
#endif
			SDL_DestroyWindow(window->window_handle);
			window->~Window();
			PE_free(window);
		}

		void CreateGlProgram(const char* vertex_source, const char* frag_source, PE_Shader& shader) {
			GladGLContext& gl = GetGladContext();
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

		void DeleteGlProgram(PE_Shader* shader) {
			GladGLContext& gl = GetGladContext();
			gl.DeleteProgram(shader->program);
			shader->program = 0;
		}

		void CreateGlMesh(PE_RenderMesh& mesh, void* vertices, size_t vertices_size, void* indices, size_t indices_size) {
			GladGLContext& gl = GetGladContext();
			mesh.buffers[PE_RenderMesh::BUF_VERTEX] = 0;
			mesh.buffers[PE_RenderMesh::BUF_ELEMENT] = 0;
			gl.GenBuffers(PE_ARRAY_LEN(mesh.buffers), mesh.buffers);
			for (int i = 0; i < PE_ARRAY_LEN(mesh.buffers); ++i) {
				if (mesh.buffers[i] == 0) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create OpenGL buffers for PE_RenderMesh"));
					return;
				}
			}

			gl.BindBuffer(GL_ARRAY_BUFFER, mesh.buffers[PE_RenderMesh::BUF_VERTEX]);
			gl.BufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
			//// TDOD Check if data was uploaded
			//for (size_t i = 0; i < attribute_count; ++i) {
			//	PE_MeshFormatAttrib& attrib = format_attributes[i];
			//	gl.VertexAttribPointer(attrib.location, attrib.size, GlMeshAttribTypeLookup[attrib.type], attrib.normalized ? GL_TRUE : GL_FALSE, attrib.stride, reinterpret_cast<void*>(static_cast<uintptr_t>(attrib.offset)));
			//	gl.EnableVertexAttribArray(attrib.location);
			//}

			gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.buffers[PE_RenderMesh::BUF_ELEMENT]);
			// TDOO Check if data was uploaded
			gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
		}

		void DeleteGlMesh(PE_RenderMesh& mesh) {
			GladGLContext& gl = GetGladContext();
			gl.DeleteBuffers(PE_ARRAY_LEN(mesh.buffers), mesh.buffers);
		}

		template<class ValueT>
		PE_NODISCARD int PushGlCommand(CommandQueue& queue, CommandType type, const ValueT& value) {
			PE_STATIC_ASSERT(sizeof(GlCommand<ValueT>) < sizeof(GlCommandBuffer::data), PE_TEXT("GlCommand Too Big for GlCommandBuffer"));
			constexpr size_t command_size = sizeof(GlCommand<ValueT>);
			size_t active_buffer_remaining = sizeof(GlCommandBuffer::data) - (reinterpret_cast<uintptr_t>(queue.active_buffer->tail) - reinterpret_cast<uintptr_t>(queue.active_buffer->data));
			if (active_buffer_remaining < command_size) {
				CommandBuffer* buf = static_cast<CommandBuffer*>(PE_malloc(sizeof(CommandBuffer)));
				if (!buf) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate OpenGL command buffer"));
					return PE_ERROR_OUT_OF_MEMORY;
				}

				buf->tail = buf->data;
				buf->next = nullptr;
				queue.active_buffer->next = buf;
				queue.active_buffer = buf;
			}

			new(static_cast<void*>(queue.active_buffer->tail)) Command<ValueT>(type, value);
			PE_STATIC_ASSERT(alignof(Command<ValueT>) <= sizeof(queue.active_buffer->tail[0]) &&
				sizeof(queue.active_buffer->tail[0]) % alignof(Command<ValueT>) == 0,
				PE_TEXT("Command can not be aligned to command buffer data type (8 bytes)"));
			PE_STATIC_ASSERT(command_size % sizeof(queue.active_buffer->tail[0]) == 0, PE_TEXT("Command size is not a multiple of command buffer data type (8 bytes)"));
			queue.active_buffer->tail += command_size / 8;
			PE_DEBUG_ASSERT(reinterpret_cast<uintptr_t>(queue.active_buffer->data) + sizeof(CommandBuffer::data) >= reinterpret_cast<uintptr_t>(queue.active_buffer->tail), PE_TEXT("CommandBuffer Overrun"));
			return PE_ERROR_NONE;
		}

		int ExecuteGlCommandBuffer(GladGLContext& gl, CommandExecutionState& state, CommandBuffer& buffer) {
			uint8_t* buffer_head = reinterpret_cast<uint8_t*>(buffer.data);
			while (reinterpret_cast<uintptr_t>(buffer_head) < reinterpret_cast<uintptr_t>(buffer.tail))
			{
				// uint64_t is just a dummy type to allow reading of the true size and type of the current command
				Command<uint64_t>* cur_cmd = reinterpret_cast<Command<uint64_t>*>(buffer_head);
				switch (cur_cmd->type)
				{
				case GL_CMD_SHADER: {
					GLuint program = reinterpret_cast<Command<GLuint>*>(cur_cmd)->value;
					if (state.program != program) {
						gl.UseProgram(program);
						state.program = program;
					}
				} break;
				case GL_CMD_MESH:
				{
					PE_RenderMesh& mesh = reinterpret_cast<Command<PE_RenderMesh>*>(cur_cmd)->value;


					if (state.mesh_vao != mesh.vao) {
						gl.BindVertexArray(mesh.vao);
						gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.buffers[PE_RenderMesh::BUF_ELEMENT]);
						state.mesh_vao = mesh.vao;
						state.index_type = GlIndexTypeLookup[mesh.index_type];
					}
				} break;
				case GL_CMD_DRAW: {
					std::pair<size_t, size_t>& pair = reinterpret_cast<Command<std::pair<size_t, size_t>>*>(cur_cmd)->value;
					gl.DrawElements(GL_TRIANGLES, pair.first, state.index_type, reinterpret_cast<void*>(pair.second));
				} break;
				case GL_CMD_UNIFORM_VALUE:
					PE_DEBUG_ASSERT(false, PE_TEXT("ExecuteGlCommandBuffer GL_CMD_UNIFORM_VALUE not implemented"));
					break;
				case GL_CMD_CULL_FACE: {
					PE_CullMode mode = reinterpret_cast<GlCommand<PE_CullMode>*>(cur_cmd)->value;
					if (mode == PE_CULL_NONE) {
						gl.Disable(GL_CULL_FACE);
					}
					else {
						gl.Enable(GL_CULL_FACE);
						gl.CullFace(GlCullFaceLookup[mode]);
					}
				} break;
				default:
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to execute OpenGL command buffer, unknow command type: %") SDL_PRIu32, cur_cmd->type);
					return PE_ERROR_INVALID_PARAMETERS;
					break;
				}

				buffer_head += cur_cmd->cmd_size;
			}

			return PE_ERROR_NONE;
		}

		void ExecuteGlCommandQueues(Window* target, CommandQueue** queues, size_t queue_count, int& return_value) {
			CommandExecutionState state;
			memset(&state, 0, sizeof(state));			
#if PE_GL_SINGLE_CONTEXT
			int res = SDL_GL_MakeCurrent(target_window, gl.userptr);
			if (res != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed execute command queue, failed set window context active: %s"), SDL_GetError());
				return_value = PE_ERROR_GENERAL;
				return;
			}
#else
			target->glad_render_context.BindVertexArray(target->opengl_render_vao);
#endif
			int w, h;
			SDL_GetWindowSizeInPixels(target->window_handle, &w, &h);
			target->glad_render_context.Viewport(0, 0, w, h);
			target->glad_render_context.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			// TODO color, scissor and, swap interval should be taken from some kind of Camera struct
			target->glad_render_context.ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			target->glad_render_context.Clear(GL_COLOR_BUFFER_BIT);
			SDL_GL_SetSwapInterval(0);

			for (size_t i = 0; i < queue_count; ++i) {
				CommandBuffer* buf = &(queues[i]->command_buffer);
				while (buf)
				{
					int rtn = ExecuteGlCommandBuffer(target->glad_render_context, state, *buf);
					if (rtn != 0) {
						return_value = rtn;
						return;
					}

					buf = buf->next;
				}
			}

			SDL_GL_SwapWindow(target->window_handle);
			return_value = PE_ERROR_NONE;
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
		int PE_INTERNAL_InitGraphicsSystemImp() {
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

			SDL_GL_GetSwapInterval(0);

			// Copy function pointers into GraphicsCommands struct
#define PE_GENERATED_GRAPHICS_API(rc, fn, params, args, ret) self->##fn = fn##Imp;
#include "PE_generated_graphics_api.h"
#undef PE_GENERATED_GRAPHICS_API
			return PE_ERROR_NONE;
	}

		void PE_INTERNAL_QuitGraphicsSystemImp() {
			if (!self) {
				return;
			}

			if (self->init) {
				self->~OpenGlGraphicsImp();
			}

			PE_free(self);
			self = nullptr;
		}

		PE_Window* PE_CreateWindowImp(char* title, int width, int height, Uint32 flags) {
			flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
			Window* window = static_cast<Window*>(PE_malloc(sizeof(Window)));
			if (!window) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate window"));
				return;
			}

			new(window) Window();
			PE_EventLoop.PushBlockingTWork<CreateSdlWindow>(title, width, height, flags, *window);
			if (!window->window_handle) {
				window->~Window();
				PE_free(window);
				return nullptr;
			}

			return static_cast<PE_Window*>(window);
		}

		void PE_DestroyWindowImp(PE_Window* window) {
			if (window) {
				PE_EventLoop.PushBlockingTWork<DestroySdlWindow>(*static_cast<Window*>(window));
			}
		}

		PE_Shader* PE_LoadShaderImp(const char* name) {
			PE_GL_REQUIRE_CTX(PE_TEXT("You must create a window before calling LoadShader"), nullptr);
			std::lock_guard cache_lock(self->shader_cache_mutex);
			const std::string* name_ptr = nullptr;
			try {
				auto cache_res = self->shader_cache.insert({ name, GlShader() });
				name_ptr = &cache_res.first->first;
				GlShader& shader = cache_res.first->second;
				if (shader.loaded) {
					shader.ref_count += 1;
					return reinterpret_cast<PE_Shader*>(&shader);
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
				return reinterpret_cast<PE_Shader*>(&shader);
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

		void PE_UnloadShaderImp(PE_Shader* shader) {
			if (shader) {
				GlShader* gl_shader = reinterpret_cast<GlShader*>(shader);
				if (--gl_shader->ref_count == 0) {
					std::lock_guard cache_lock(self->shader_cache_mutex);
					if (gl_shader->ref_count == 0) {
						RunLoadWork<DeleteGlProgram>(gl_shader);
						self->shader_cache.erase(*(gl_shader->cache_key));
					}
				}
			}
		}

		PE_RenderMesh* PE_CreateRenderMeshImp(void* vertices, size_t vertices_size, void* indices, size_t indices_size, PE_IndexType index_type, PE_MeshFormatAttrib* format_attributes, size_t attribute_count) {
			if (!format_attributes || attribute_count == 0 || !vertices || vertices_size == 0 || !indices || indices_size == 0 || index_type >= PE_INDEX_ENUM_VALUE_COUNT) {
				PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Invalid parameters sent to UploadMeshVertices"));
				return nullptr;
			}

			PE_STATIC_ASSERT(alignof(PE_RenderMesh) >= alignof(RenderMeshAttrib) && alignof(PE_RenderMesh) % alignof(RenderMeshAttrib) == 0, PE_TEXT("RenderMeshAttrib has invalid alignment"));
			PE_RenderMesh* mesh = static_cast<PE_RenderMesh*>(PE_malloc(sizeof(PE_RenderMesh) + (sizeof(RenderMeshAttrib) * attribute_count)));
			if (!mesh) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate PE_RenderMesh"));
				return nullptr;
			}

			mesh->attribs = reinterpret_cast<RenderMeshAttrib*>(&(mesh[1]));
			mesh->attrib_count = attribute_count;
			mesh->index_type = index_type;
			for (size_t i = 0; i < attribute_count; ++i) {
				ConvertPEMeshFormatAttrib(format_attributes[i], mesh->attribs[i]);
			}

			PE_GL_WORKER.PushBlockingTWork<CreateGlMesh>(*mesh, vertices, vertices_size, indices, indices_size);
			if (mesh->buffers[PE_RenderMesh::BUF_VERTEX] == 0 || mesh->buffers[PE_RenderMesh::BUF_ELEMENT] == 0) {
				PE_DestroyRenderMeshImp(mesh);
				return nullptr;
			}

			return mesh;
		}

		void PE_DestroyRenderMeshImp(PE_RenderMesh* mesh) {
			if (!mesh) {
				return;
			}

			if (mesh->buffers[PE_RenderMesh::BUF_VERTEX] != 0 || mesh->buffers[PE_RenderMesh::BUF_ELEMENT] != 0) {
				PE_GL_WORKER.PushBlockingTWork<DeleteGlMesh>(*mesh);
			}

			PE_free(mesh);
		}

		PE_GraphicsCommandQueue* PE_CreateGraphicsCommandQueueImp() {
			GlCommandQueue* cmd_queue = static_cast<GlCommandQueue*>(PE_malloc(sizeof(GlCommandQueue)));
			if (!cmd_queue) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate OpenGL command queue"));
				return nullptr;
			}

			cmd_queue->active_buffer = &cmd_queue->command_buffer;
			cmd_queue->active_buffer->tail = cmd_queue->active_buffer->data;
			cmd_queue->command_buffer.next = nullptr;
			return reinterpret_cast<PE_GraphicsCommandQueue*>(cmd_queue);
		}

		void PE_ClearGraphicsCommandQueueImp(PE_GraphicsCommandQueue* queue) {
			if (!queue) {
				return;
			}

			GlCommandQueue* cmd_queue = reinterpret_cast<GlCommandQueue*>(queue);
			GlCommandBuffer* buf = &cmd_queue->command_buffer;
			while (buf != nullptr)
			{
				buf->tail = buf->data;
				buf = buf->next;
			}
		}

		void PE_DestroyGraphicsCommandQueueImp(PE_GraphicsCommandQueue* queue) {
			if (!queue) {
				return;
			}

			GlCommandQueue* cmd_queue = reinterpret_cast<GlCommandQueue*>(queue);
			GlCommandBuffer* sub_buffer = cmd_queue->command_buffer.next;
			while (sub_buffer != nullptr)
			{
				GlCommandBuffer* buf = sub_buffer;
				sub_buffer = buf->next;
				PE_free(buf);
			}

			PE_free(cmd_queue);
		}

		int PE_RenderToWindowImp(SDL_Window* target_window, PE_GraphicsCommandQueue** queues, size_t queue_count) {
			if (!queues || !target_window) {
				return PE_ERROR_INVALID_PARAMETERS;
			}

			if (queue_count == 0) {
				return PE_ERROR_NONE;
			}

			for (size_t i = 0; i < queue_count; ++i) {
				if (queues[i] == nullptr) {
					return PE_ERROR_INVALID_PARAMETERS;
				}
		}

			int return_value = 0;
#if PE_GL_SINGLE_CONTEXT
			RunMainWork<ExecuteGlCommandQueues>(self->primary_ctx, target_window, reinterpret_cast<GlCommandQueue**>(queues), queue_count, return_value);
#else
			WindowContext* window_ctx = nullptr;
			SDL_PropertiesID props = SDL_GetWindowProperties(target_window);
			if (props != 0) {
				window_ctx = static_cast<WindowContext*>(SDL_GetProperty(props, PE_GL_CTX_KEY, nullptr));
			}
			else {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed execute command queue, failed to get window context: %s"), SDL_GetError());
				return PE_ERROR_GENERAL;
			}

			if (!window_ctx) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed execute command queue, window missing context"));
				return PE_ERROR_GENERAL;
			}

			RunWorker<ExecuteGlCommandQueues>(window_ctx->worker, window_ctx->glad_ctx, target_window, reinterpret_cast<GlCommandQueue**>(queues), queue_count, return_value);
#endif
			return return_value;
}

		int PE_GraphicsCommandSetShaderImp(PE_GraphicsCommandQueue* queue, PE_Shader* shader) {
			if (!queue || !shader) {
				return PE_ERROR_INVALID_PARAMETERS;
			}

			return PushGlCommand(*reinterpret_cast<GlCommandQueue*>(queue), GlCommandType::GL_CMD_SHADER, reinterpret_cast<GlShader*>(shader)->program);
		}

		int PE_GraphicsCommandSetMeshImp(PE_GraphicsCommandQueue* queue, PE_RenderMesh* mesh) {
			if (!queue || !mesh) {
				return PE_ERROR_INVALID_PARAMETERS;
			}

			return PushGlCommand(*reinterpret_cast<GlCommandQueue*>(queue), GlCommandType::GL_CMD_MESH, *reinterpret_cast<GlRenderMesh*>(mesh));
		}

		int PE_GraphicsCommandDrawMeshImp(PE_GraphicsCommandQueue* queue, size_t index_count, size_t index_offset) {
			if (!queue || index_count % 3 != 0) {
				return PE_ERROR_INVALID_PARAMETERS;
			}

			if (index_count == 0) {
				return PE_ERROR_NONE;
			}

			return PushGlCommand(*reinterpret_cast<GlCommandQueue*>(queue), GlCommandType::GL_CMD_DRAW, std::pair<size_t, size_t>(index_count, index_offset));
		}

		int PE_GraphicsCommandCullImp(PE_GraphicsCommandQueue* queue, PE_CullMode mode) {
			if (!queue || mode >= PE_CULL_ENUM_VALUE_COUNT) {
				return PE_ERROR_INVALID_PARAMETERS;
			}

			return PushGlCommand(*reinterpret_cast<GlCommandQueue*>(queue), GlCommandType::GL_CMD_CULL_FACE, mode);
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
		self->PE_INTERNAL_InitGraphicsSystem = PE_INTERNAL_InitGraphicsSystemImp;
		self->PE_INTERNAL_QuitGraphicsSystem = PE_INTERNAL_QuitGraphicsSystemImp;
		return static_cast<GraphicsCommands*>(self);
	}
}
#endif
