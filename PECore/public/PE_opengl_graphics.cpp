//#ifndef PE_DISABLE_OPENGL
//#include "gl_graphics/PE_opengl_graphics.h"
//#include "gl_graphics/PE_gl_defines.h"
//#include "gl_graphics/PE_gl_types.h"
//#include "gl_graphics/PE_gl_shader.h"
//#include "gl_graphics/PE_gl_window.h"
//#include "gl_graphics/PE_gl_mesh.h"
//#include "PE_graphics_implementation.h"
//#include <filesystem>
//
//#define PE_GL_REQUIRE_CTX(msg, rtn)
//
//// Returns load thread worker or PE_EventLoop if there is no load worker
//// and returns glad load context or glad render context if there is no load context
//#if PE_GL_BACKGROUND_LOADING
//#define PE_GL_WORKER this_->load_thread_worker
//#define PE_GL_GLAD_CTX this_->load_context
//#else
//#define PE_GL_WORKER PE_EventLoop
//#define PE_GL_CTX this_->main_context
//#endif
//
//namespace pecore::graphics::open_gl {
//	namespace {
//		struct OpenGlGraphicsCommands : public GraphicsCommands {
//			GlContextWrapper main_context;
//			Window* active_main_window;
//#if PE_GL_BACKGROUND_LOADING
//			GlContextWrapper load_context;
//			ThreadWorker load_thread_worker;
//#endif
//			ShaderCache shader_cache;
//		};
//
//		OpenGlGraphicsCommands* this_;
//
//		// Create graphics *Gl function declaration
//#define PE_GENERATED_GRAPHICS_API(rc, fn, params, args, ret) rc fn##Gl params;
//#include "PE_generated_graphics_api.h"
//#undef PE_GENERATED_GRAPHICS_API
//
//		PE_Window* PE_CreateWindowGl(char* title, int width, int height, Uint32 flags) {
//			return static_cast<PE_Window*>(this_->active_main_window = Window::CreateWindow(title, width, height, flags));
//		}
//
//		void PE_DestroyWindowGl(PE_Window* window) {
//			if (window) {
//				Window* win = static_cast<Window*>(window);
//				if (this_->active_main_window == win) {
//					this_->active_main_window = nullptr;
//				}
//
//				win->DestroyWindow();
//			}
//		}
//
//		PE_Shader* PE_LoadShaderGl(const char* name) PE_NOEXCEPT {
//			PE_GL_REQUIRE_CTX(PE_TEXT("You must create a window before calling PE_LoadShader"), nullptr);
//			PE_Shader* shader = nullptr;
//			try {
//				this_->shader_cache.Lock();
//			}
//			catch (const std::exception& e) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to lock shader cache, error: %s"), e.what());
//				return nullptr;
//			}
//			catch (...) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to lock shader cache, unknown error"));
//				return nullptr;
//			}
//
//			try {
//				shader = this_->shader_cache.GetOrCreateShaderUnsafe(name);
//				if (!shader->loaded) {
//					std::filesystem::path shader_path(std::filesystem::path(PE_TEXT("res")) / PE_TEXT("shaders") / name);
//					shader_path.concat(PE_TEXT(".pegls"));
//					PE_LogVerbose(PE_LOG_CATEGORY_RENDER, PE_TEXT("Loading Shader %s from %s"), name, shader_path.generic_string().c_str());
//
//					ShaderCache::ShaderSource source;
//					this_->shader_cache.LoadShaderSource(shader_path, source);
//					PE_GL_WORKER.PushBlockingTWork<ShaderCache::CompileShaderProgram>(PE_GL_CTX.gl, *shader, source);
//					if (shader->program == 0) {
//						throw std::exception("Failed to compile shader");
//					}
//				}
//			}
//			catch (const PE_ErrorCode& e) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader file, error code: %") SDL_PRIs32, e);
//			}
//			catch (const std::exception& e) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader file, error: %s"), e.what());
//			}
//			catch (...) {
//				PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to load shader file, unknown error"));
//			}
//
//			if (shader && !shader->loaded) {
//				ShaderCache::ReleaseShader(*shader);
//				try {
//					this_->shader_cache.DestroyShaderUnsafe(*shader);
//				}
//				catch (const std::exception& e) {
//					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to remove invalid shader from cache, error: %s"), e.what());
//				}
//				catch (...) {
//					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to remove invalid shader from cache, unknown error"));
//				}
//
//				shader = nullptr;
//			}
//
//			this_->shader_cache.Unlock();
//			return shader;
//		}
//
//		void PE_CALL PE_UnloadShaderGl(PE_Shader* shader) PE_NOEXCEPT {
//			if (shader) {
//				try {
//					this_->shader_cache.Lock();
//				}
//				catch (const std::exception& e) {
//					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to unload shader, failed to lock shader cache, error: %s"), e.what());
//					return;
//				}
//				catch (...) {
//					PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to unload shader, failed to lock shader cache, unknown error"));
//					return;
//				}
//
//				if (ShaderCache::ReleaseShader(*shader)) {
//					PE_GL_WORKER.PushBlockingTWork<ShaderCache::DeleteShaderProgram>(PE_GL_CTX.gl, *shader);
//					this_->shader_cache.DestroyShaderUnsafe(*shader);
//				}
//
//				this_->shader_cache.Unlock();
//			}
//		}
//
//		PE_RenderMesh* PE_CreateRenderMeshGl(void* vertices, size_t vertices_size, void* indices, size_t indices_size) {
//			return PE_RenderMesh::CreateRenderMesh(PE_GL_WORKER, PE_GL_CTX, vertices, vertices_size, indices, indices_size);
//		}
//
//		void PE_DestroyRenderMeshGl(PE_RenderMesh* mesh) {
//			if (mesh) {
//				mesh->DestroyMesh(PE_GL_WORKER, PE_GL_CTX);
//			}
//		}
//	}
//}
//
//pecore::graphics::GraphicsCommands* pecore::graphics::AllocateOpenGlImplementation() {
//	return nullptr;
//}
//#endif // !PE_DISABLE_OPENGL