#include "opengl_graphics/PE_opengl_window.h"
#include "PE_log.h"
#include "PE_memory.h"
#include "PE_worker_thread.h"
#include "PE_event_loop.h"
#include "PE_global.h"
#include "glad/gl.h"

namespace pe::internal::opengl {
	GlWindow::GlWindow() {
		window_handle = nullptr;
#if PE_GL_CONTEXT_PER_WINDOW
		context.opengl_context = nullptr;
		context.vertex_array_object = 0;
		context.init = false;
#endif
	}

	GlWindow::~GlWindow() {
		PE_DEBUG_ASSERT(window_handle == nullptr, PE_TEXT("SDL_Window was not nullptr when GlWindow destructor was called"));
#if PE_GL_CONTEXT_PER_WINDOW
		PE_DEBUG_ASSERT(context.opengl_context == nullptr, PE_TEXT("SDL_GLContext was not nullptr when GlWindow destructor was called"));
#endif
	}

	GlWindow* GlWindow::Create(const char* title, int width, int height, Uint32 flags) PE_NOEXCEPT {
		flags = (flags & ~(SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) | SDL_WINDOW_OPENGL;
		GlWindow* window = static_cast<GlWindow*>(PE_malloc(sizeof(GlWindow)));
		if (!window) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to allocate window"));
			return nullptr;
		}

		new(window) GlWindow();
		// Execute on main/event thread
		Global.event_loop.PushBlockingTWork<CreateWindowHandle>(window, title, width, height, flags);
		if (window->window_handle == nullptr) {
			window->~GlWindow();
			PE_free(window);
			return nullptr;
		}

		return window;
	}

	void GlWindow::OnDrop() PE_NOEXCEPT {
#if PE_GL_CONTEXT_PER_WINDOW
		worker.PushBlockingTWork<DestroyContext>(this);
#endif
		Global.event_loop.PushBlockingTWork<DestroyWindowHandle>(this);
		this->~GlWindow();
		PE_free(this);
	}

	SDL_Window* GlWindow::GetSDLHandle() const {
		return window_handle;
	}

	void GlWindow::CreateWindowHandle(const char* title, int width, int height, Uint32 flags) PE_NOEXCEPT {
		window_handle = SDL_CreateWindow(title, width, height, flags);
		if (!window_handle) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, error: %s"), SDL_GetError());
			return;
		}

		SDL_GLContext cur_ctx = SDL_GL_GetCurrentContext();
		// We can not continue without a main context
		PE_ASSERT(cur_ctx != nullptr, PE_TEXT("Invalid main OpenGl context"));
#if PE_GL_CONTEXT_PER_WINDOW
		// TODO Does this maintain the current context if it fails? We assume it does
		context.opengl_context = SDL_GL_CreateContext(window_handle);
		if (context.opengl_context == nullptr) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window context, error: %s"), SDL_GetError());
			SDL_DestroyWindow(window_handle);
			window_handle = nullptr;
			return;
		}

		int window_gl_version = gladLoadGLContext(&context.gl, SDL_GL_GetProcAddress);
		PE_LogDebug(PE_LOG_CATEGORY_RENDER, PE_TEXT("Window GL Version: %") SDL_PRIs32 PE_TEXT(".%") SDL_PRIs32, GLAD_VERSION_MAJOR(window_gl_version), GLAD_VERSION_MINOR(window_gl_version));
		context.gl.CreateVertexArrays(1, &context.vertex_array_object);
		if (SDL_GL_MakeCurrent(window_handle, cur_ctx) != 0) {
			// We can not continue without a main context
			PE_ASSERT(false, PE_TEXT("Failed set main OpenGl context active, error: %s"), SDL_GetError());
		}

		if (context.vertex_array_object == 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window vertex array object"));
			SDL_GL_DeleteContext(context.opengl_context);
			SDL_DestroyWindow(window_handle);
			window_handle = nullptr;
			return;
		}

		int return_code;
		worker.PushBlockingTWork<SetContextActive>(this, return_code);
		if (return_code != 0) {
			SDL_GL_DeleteContext(context.opengl_context);
			SDL_DestroyWindow(window_handle);
			window_handle = nullptr;
		}
#else
		if (SDL_GL_MakeCurrent(window_handle, cur_ctx) != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to create window, context incompatible with window, error: %s"), SDL_GetError());
			SDL_DestroyWindow(window_handle);
			window_handle = nullptr;
		}
#endif
	}

	void GlWindow::DestroyWindowHandle() {
		SDL_DestroyWindow(window_handle);
		window_handle = nullptr;
	}

#if PE_GL_CONTEXT_PER_WINDOW
	// Note: return_code is a SDL return value
	void GlWindow::SetContextActive(int& return_code) {
		if (return_code = SDL_GL_MakeCurrent(window_handle, context.opengl_context) != 0) {
			PE_LogError(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed set window OpenGl context active, error: %s"), SDL_GetError());
		}
	}

	void GlWindow::DestroyContext() {
		// TODO Destroy VAO
		if (SDL_GL_DeleteContext(context.opengl_context) != 0) {
			PE_LogWarn(PE_LOG_CATEGORY_RENDER, PE_TEXT("Failed to delete window OpenGl context, error: $s"), SDL_GetError());
		}

		context.opengl_context = nullptr;
	}
#endif
}