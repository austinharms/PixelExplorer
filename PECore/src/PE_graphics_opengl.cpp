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
#define PE_GL_SINGLE_CONTEXT 1
#endif // !PE_GL_SINGLE_CONTEXT

// Should we create and use a worker thread and context to load OpenGL data
#ifndef PE_GL_BACKGROUND_LOADING
#define PE_GL_BACKGROUND_LOADING 0
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
		constexpr GLenum GlIndexTypeLookup[] = { GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };
		PE_STATIC_ASSERT(PE_ARRAY_LEN(GlIndexTypeLookup) == PE_INDEX_ENUM_VALUE_COUNT, PE_TEXT("GlIndexTypeLookup array need to be updated"));
		constexpr GLenum GlMeshAttribTypeLookup[] = { GL_FLOAT, GL_HALF_FLOAT, GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT };
		PE_STATIC_ASSERT(PE_ARRAY_LEN(GlMeshAttribTypeLookup) == PE_MESH_ATR_ENUM_VALUE_COUNT, PE_TEXT("GlMeshAttribTypeLookup array need to be updated"));
		constexpr GLenum GlCullFaceLookup[] = { GL_BACK, GL_FRONT, GL_FRONT_AND_BACK, GL_BACK };
		PE_STATIC_ASSERT(PE_ARRAY_LEN(GlCullFaceLookup) == PE_CULL_ENUM_VALUE_COUNT, PE_TEXT("GlCullFaceLookup array need to be updated"));

		struct GlShader {
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

		struct GlRenderMesh {
			enum {
				BUF_VERTEX = 0,
				BUF_ELEMENT = 1,
			};

			GLuint buffers[2];
			GLuint vao;
			PE_IndexType index_type;
		};

		enum GlCommandType : uint32_t
		{
			GL_CMD_SHADER = 0,
			GL_CMD_MESH,
			GL_CMD_DRAW,
			GL_CMD_UNIFORM_VALUE,
			GL_CMD_CULL_FACE,
		};

		template<class ValueT>
		struct GlCmdPad {
			enum { val = sizeof(ValueT) % 8 };
		};

		template<class ValueT, size_t PAD>
		struct GlCommandBase {
			constexpr GlCommandBase(GlCommandType cmd_type, const ValueT& cmd_value) :
				cmd_size(sizeof(ValueT) + sizeof(uint32_t) + sizeof(GlCommandType) + PAD),
				type(cmd_type),
				value(cmd_value) {
			}

			uint32_t cmd_size;
			GlCommandType type;
			ValueT value;
			uint8_t pad[PAD];
		};

		template<class ValueT>
		struct GlCommandBase<ValueT, 0> {
			constexpr GlCommandBase(GlCommandType cmd_type, const ValueT& cmd_value) :
				cmd_size(sizeof(ValueT) + sizeof(uint32_t) + sizeof(GlCommandType)),
				type(cmd_type),
				value(cmd_value) {
			}

			uint32_t cmd_size;
			GlCommandType type;
			ValueT value;
		};

		template<class ValueT>
		struct GlCommand : public GlCommandBase<ValueT, GlCmdPad<ValueT>::val> {
			using GlCommandBase<ValueT, GlCmdPad<ValueT>::val>::GlCommandBase;
		};

		struct GlCommandBuffer {
			GlCommandBuffer* next;
			uint64_t* tail;
			uint64_t data[2048];
		};

		struct GlCommandQueue {
			GlCommandBuffer command_buffer;
			GlCommandBuffer* active_buffer;
		};

		struct GlCommandExecutionState {
			GLuint program;
			GLuint mesh_vao;
			GLenum index_type;
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
			self->background_worker.PushBlockingWork(WorkWrapper<decltype(work), work, decltype(params)>, static_cast<void*>(&params));
#else
			EventLoop.PushBlockingWork(WorkWrapper<decltype(work), work, decltype(params)>, static_cast<void*>(&params));
#endif
		}

		// Helper function that runs functions on the main/event thread
		template<auto work, class... Args>
		PE_FORCEINLINE void RunMainWork(Args&&... args) {
			auto params = std::forward_as_tuple(args ...);
			EventLoop.PushBlockingWork(WorkWrapper<decltype(work), work, decltype(params)>, static_cast<void*>(&params));
		}

		// Helper function that runs functions on the provided worker
		template<auto work, class... Args>
		PE_FORCEINLINE void RunWorker(ThreadWorker& worker, Args&&... args) {
			auto params = std::forward_as_tuple(args ...);
			worker.PushBlockingWork(WorkWrapper<decltype(work), work, decltype(params)>, static_cast<void*>(&params));
		}

		PE_FORCEINLINE GladGLContext& GetBackgroundGladContext() {
#if PE_GL_BACKGROUND_LOADING
			return self->background_ctx;
#else
			return self->primary_ctx;
#endif
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
			GladGLContext& gl = GetBackgroundGladContext();
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
			GladGLContext& gl = GetBackgroundGladContext();
			gl.DeleteProgram(shader->program);
			shader->program = 0;
		}

		void CreateGlMesh(GlRenderMesh& mesh, PE_MeshFormatAttrib* format_attributes, size_t attribute_count, void* vertices, size_t vertices_size, void* indices, size_t indices_size) {
			GladGLContext& gl = GetBackgroundGladContext();
			mesh.vao = 0;
			mesh.buffers[GlRenderMesh::BUF_VERTEX] = 0;
			mesh.buffers[GlRenderMesh::BUF_ELEMENT] = 0;
			gl.GenVertexArrays(1, &mesh.vao);
			if (mesh.vao == 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create OpenGL vertex array (RenderMesh)"));
				return;
			}

			gl.GenBuffers(PE_ARRAY_LEN(mesh.buffers), mesh.buffers);
			for (int i = 0; i < PE_ARRAY_LEN(mesh.buffers); ++i) {
				if (mesh.buffers[i] == 0) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create OpenGL buffer (RenderMesh)"));
					gl.DeleteVertexArrays(1, &mesh.vao);
					mesh.vao = 0;
					return;
				}
			}

			gl.BindVertexArray(mesh.vao);
			gl.BindBuffer(GL_ARRAY_BUFFER, mesh.buffers[GlRenderMesh::BUF_VERTEX]);
			gl.BufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
			// TDOD Check if data was uploaded
			for (size_t i = 0; i < attribute_count; ++i) {
				PE_MeshFormatAttrib& attrib = format_attributes[i];
				gl.VertexAttribPointer(attrib.location, attrib.size, GlMeshAttribTypeLookup[attrib.type], attrib.normalized ? GL_TRUE : GL_FALSE, attrib.stride, reinterpret_cast<void*>(static_cast<uintptr_t>(attrib.offset)));
				gl.EnableVertexAttribArray(attrib.location);
			}

			gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.buffers[GlRenderMesh::BUF_ELEMENT]);
			// TDOO Check if data was uploaded
			gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
		}

		void DeleteGlMesh(GlRenderMesh& mesh) {
			GladGLContext& gl = GetBackgroundGladContext();
			gl.DeleteVertexArrays(1, &mesh.vao);
			gl.DeleteBuffers(PE_ARRAY_LEN(mesh.buffers), mesh.buffers);
		}

		template<class ValueT>
		PE_NODISCARD int PushGlCommand(GlCommandQueue& queue, GlCommandType type, const ValueT& value) {
			PE_STATIC_ASSERT(sizeof(GlCommand<ValueT>) < sizeof(GlCommandBuffer::data), PE_TEXT("GlCommand Too Big for GlCommandBuffer"));
			constexpr size_t command_size = sizeof(GlCommand<ValueT>);
			size_t active_buffer_remaining = sizeof(GlCommandBuffer::data) - (reinterpret_cast<uintptr_t>(queue.active_buffer->tail) - reinterpret_cast<uintptr_t>(queue.active_buffer->data));
			if (active_buffer_remaining < command_size) {
				GlCommandBuffer* buf = static_cast<GlCommandBuffer*>(PE_malloc(sizeof(GlCommandBuffer)));
				if (!buf) {
					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate OpenGL command buffer"));
					return PE_ERROR_OUT_OF_MEMORY;
				}

				buf->tail = buf->data;
				buf->next = nullptr;
				queue.active_buffer->next = buf;
				queue.active_buffer = buf;
			}

			new (static_cast<void*>(queue.active_buffer->tail)) GlCommand<ValueT>(type, value);
			PE_STATIC_ASSERT(alignof(GlCommand<ValueT>) <= sizeof(queue.active_buffer->tail[0]) &&
				sizeof(queue.active_buffer->tail[0]) % alignof(GlCommand<ValueT>) == 0,
				PE_TEXT("GlCommand can not be aligned to command buffer data type (8 bytes)"));
			PE_STATIC_ASSERT(command_size % sizeof(queue.active_buffer->tail[0]) == 0, PE_TEXT("GlCommand size is not a multiple of command buffer data type (8 bytes)"));
			queue.active_buffer->tail += command_size / 8;
			PE_DEBUG_ASSERT(reinterpret_cast<uintptr_t>(&(queue.active_buffer->data[0])) + sizeof(GlCommandBuffer::data) >= reinterpret_cast<uintptr_t>(queue.active_buffer->tail), PE_TEXT("GlCommandBuffer Overrun"));
			return PE_ERROR_NONE;
		}

		int ExecuteGlCommandBuffer(GladGLContext& gl, GlCommandExecutionState& state, GlCommandBuffer& buffer) {
			uint8_t* buffer_head = reinterpret_cast<uint8_t*>(buffer.data);
			while (reinterpret_cast<uintptr_t>(buffer_head) < reinterpret_cast<uintptr_t>(buffer.tail))
			{
				// uint64_t is just a dummy type to allow reading of the true size and type of the current command
				GlCommand<uint64_t>* cur_cmd = reinterpret_cast<GlCommand<uint64_t>*>(buffer_head);
				switch (cur_cmd->type)
				{
				case GL_CMD_SHADER: {
					GLuint program = reinterpret_cast<GlCommand<GLuint>*>(cur_cmd)->value;
					if (state.program != program) {
						gl.UseProgram(program);
						state.program = program;
					}
				} break;
				case GL_CMD_MESH:
				{
					GlRenderMesh& mesh = reinterpret_cast<GlCommand<GlRenderMesh>*>(cur_cmd)->value;
					if (state.mesh_vao != mesh.vao) {
						gl.BindVertexArray(mesh.vao);
						gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.buffers[GlRenderMesh::BUF_ELEMENT]);
						state.mesh_vao = mesh.vao;
						state.index_type = GlIndexTypeLookup[mesh.index_type];
					}
				} break;
				case GL_CMD_DRAW: {
					std::pair<size_t, size_t>& pair = reinterpret_cast<GlCommand<std::pair<size_t, size_t>>*>(cur_cmd)->value;
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

		void ExecuteGlCommandQueues(GladGLContext& gl, SDL_Window* target_window, GlCommandQueue** queues, size_t queue_count, int& return_value) {
			GlCommandExecutionState state;
			memset(&state, 0, sizeof(state));
#if PE_GL_SINGLE_CONTEXT
			int res = SDL_GL_MakeCurrent(target_window, gl.userptr);
			if (res != 0) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed execute command queue, failed set window context active: %s"), SDL_GetError());
				return_value = PE_ERROR_GENERAL;
				return;
			}
#endif
			int w, h;
			SDL_GetWindowSizeInPixels(target_window, &w, &h);
			gl.Viewport(0, 0, w, h);
			gl.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			// TODO color, scissor and, swap interval should be taken from some kind of Camera struct
			gl.ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			gl.Clear(GL_COLOR_BUFFER_BIT);
			SDL_GL_SetSwapInterval(0);

			for (size_t i = 0; i < queue_count; ++i) {
				GlCommandBuffer* buf = &(queues[i]->command_buffer);
				while (buf)
				{
					int rtn = ExecuteGlCommandBuffer(gl, state, *buf);
					if (rtn != 0) {
						return_value = rtn;
						return;
					}

					buf = buf->next;
				}
			}

			SDL_GL_SwapWindow(target_window);
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

		SDL_Window* PE_CreateWindowImp(char* title, int width, int height, Uint32 flags) {
			SDL_Window* window = nullptr;
			flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
			RunMainWork<CreateSdlWindow>(title, width, height, flags, window);
			return window;
		}

		void PE_DestroyWindowImp(SDL_Window* window) {
			if (window) {
				RunMainWork<DestroySdlWindow>(window);
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

			GlRenderMesh* mesh = static_cast<GlRenderMesh*>(PE_malloc(sizeof(GlRenderMesh)));
			if (!mesh) {
				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate OpenGL RenderMesh"));
				return nullptr;
			}

			mesh->index_type = index_type;
			RunLoadWork<CreateGlMesh>(*mesh, format_attributes, attribute_count, vertices, vertices_size, indices, indices_size);
			if (mesh->vao == 0) {
				PE_free(mesh);
				return nullptr;
			}

			return reinterpret_cast<PE_RenderMesh*>(mesh);
		}

		void PE_DestroyRenderMeshImp(PE_RenderMesh* mesh) {
			if (!mesh) {
				return;
			}

			GlRenderMesh* gl_mesh = reinterpret_cast<GlRenderMesh*>(mesh);
			if (gl_mesh->vao != 0) {
				RunLoadWork<DeleteGlMesh>(*gl_mesh);
			}

			PE_free(gl_mesh);
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
