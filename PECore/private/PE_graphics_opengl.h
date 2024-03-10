#ifndef PE_DISABLE_OPENGL
#ifndef PE_OPENGL_ADAPTER_H_
#define PE_OPENGL_ADAPTER_H_
#include "PE_defines.h"
//#include "PE_worker_thread.h"6
#include "PE_graphics_implementation.h"
//#include "glad/gl.h"

//// Should all windows use one OpenGl context and render on the event loop aka main thread
//// Note: this does not ensure only one context exists,
//// if PE_OGL_BACKGROUND_LOADING is true it will still create it's own context
//#ifndef PE_OGL_SINGLE_CONTEXT
//#define PE_OGL_SINGLE_CONTEXT 0
//#endif // !PE_OGL_SINGLE_CONTEXT
//
//// Should we create and use a worker thread and context to load OpenGL data
//#ifndef PE_OGL_BACKGROUND_LOADING
//#define PE_OGL_BACKGROUND_LOADING 1
//#endif // !PE_OGL_BACKGROUND_LOADING
//
//#if PE_OGL_SINGLE_CONTEXT == 0 && PE_OGL_BACKGROUND_LOADING == 0
//#error PE_OGL_SINGLE_CONTEXT and or PE_OGL_BACKGROUND_LOADING must be enabled
//#endif

namespace pecore::graphics::implementation {
	// This only allocates the implementation and sets the Init and Quit in functions pointers
	// Returns nullptr on allocation failure
	PE_NODISCARD GraphicsCommands* PE_CALL AllocateOpenGlImplementation();

//	class OpenGlGraphicsCommands : public GraphicsCommands {
//	public:
//		OpenGlGraphicsCommands(int* ret_val);
//		virtual ~OpenGlGraphicsCommands();
//#define PE_GRAPHICS_API(rc, fn, params, args, ret)	\
//	static rc fn params;							\
//	
//#include "PE_dgapi.h"
//#undef PE_GRAPHICS_API
//
//	private:
//		struct CreateWindowParams;
//		struct CreateGLShaderParams;
//
//		static OpenGlGraphicsCommands self_;
//
//#if PE_OGL_SINGLE_CONTEXT
//		GladGLContext primary_glad_ctx_;
//		SDL_Window* active_ctx_window_;
//#endif
//#if PE_OGL_BACKGROUND_LOADING
//		GladGLContext background_glad_ctx_;
//		ThreadWorker background_load_worker_;
//#endif
//
//		// Creates a SDL_Window
//		// Note: this must be run on the PE event loop aka main thread
//		static void CreateWindowHelper(void* window_parameters /* struct CreateWindowParams* */);
//		// Internal function that gets called by CreateWindowHelper
//		void CreateWindowHelperInternal(CreateWindowParams& window_parameters /* struct CreateWindowParams* */);
//
//		// Destroys the SDL_Window
//		// Note: this must be run on the PE event loop aka main thread
//		static void DestroyWindowHelper(void* window /* SDL_Window* */);
//
//		// Sets the provided context active on the worker thread
//		static void SetGLContextHelper(void* context_parameters /* struct SetGLContextParams* */);
//
//		// Destroys a GladGLContext and the corresponding SDL_GLContext
//		// Note: it is expected that GladGLContext.userptr contains the SDL_GLContext
//		static void DestroyContextHelper(void* glad_context /* GladGLContext* */);
//
//
//		static void CreateShaderHelper(void* shader_source /* struct CreateGLShaderParams* */);
//	};
}
#endif // !PE_OPENGL_ADAPTER_H_
#endif // !PE_DISABLE_OPENGL
