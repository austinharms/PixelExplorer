#ifndef PE_DISABLE_OPENGL
#ifndef PE_OPENGL_ADAPTER_H_
#define PE_OPENGL_ADAPTER_H_
#include "PE_defines.h"
#include "PE_worker_thread.h"
#include "PE_graphics_adapter.h"
#include "glad/gl.h"

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

namespace pecore::pe_graphics::open_gl {
	class OpenGLGraphicsAdapter PE_FINAL : public GraphicsAdapterInterface {
	public:
		OpenGLGraphicsAdapter(int* ret_val);
		virtual ~OpenGLGraphicsAdapter();
#define PE_GRAPHICS_API(rc, fn, params, args, ret) rc fn params PE_OVERRIDE;
#include "PE_dgapi.h"
#undef PE_GRAPHICS_API

	private:
		struct CreateWindowParams;
#if PE_OGL_SINGLE_CONTEXT
		GladGLContext primary_glad_ctx_;
		SDL_Window* active_ctx_window_;
#endif
#if PE_OGL_BACKGROUND_LOADING
		GladGLContext background_glad_ctx_;
		ThreadWorker background_load_worker_;
#endif

		// Creates a SDL_Window
		// Note: this must be run on the PE event loop aka main thread
		static void CreateWindowHelper(void* window_parameters /* struct CreateWindowParams* */);
		// Internal function that gets called by CreateWindowHelper
		void CreateWindowHelperInternal(CreateWindowParams& window_parameters /* struct CreateWindowParams* */);

		// Destroys the SDL_Window
		// Note: this must be run on the PE event loop aka main thread
		static void DestroyWindowHelper(void* window /* SDL_Window* */);

		// Sets the provided context active on the worker thread
		static void SetGLContextHelper(void* context_parameters /* struct SetGLContextParams* */);

		// Destroys a GladGLContext and the corresponding SDL_GLContext
		// Note: it is expected that GladGLContext.userptr contains the SDL_GLContext
		static void DestroyContextHelper(void* glad_context /* GladGLContext* */);
	};
}
#endif // !PE_OPENGL_ADAPTER_H_
#endif // !PE_DISABLE_OPENGL
