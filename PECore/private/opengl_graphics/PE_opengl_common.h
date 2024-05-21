#ifndef PE_DISABLE_OPENGL
#ifndef PE_GL_GRAPHICS_GL_DEFINES_H_
#define PE_GL_GRAPHICS_GL_DEFINES_H_
#include "PE_defines.h"
#include "PE_graphics.h"
#include "PE_worker_thread.h"
#include "PE_event_loop.h"
#include "glad/gl.h"
#include "SDL_video.h"

// Should windows use there own OpenGl context and render thread
// If false all windows will render using a single context on the main/event thread
#ifndef PE_GL_CONTEXT_PER_WINDOW
#define PE_GL_CONTEXT_PER_WINDOW 0
#endif // !PE_GL_CONTEXT_PER_WINDOW

// Enables background loading using a worker thread and context
#ifndef PE_GL_BACKGROUND_LOADING
#define PE_GL_BACKGROUND_LOADING 0
#endif // !PE_GL_BACKGROUND_LOADING
#endif // !PE_GL_GRAPHICS_GL_DEFINES_H_

namespace pe::internal::open_gl {
#if PE_GL_BACKGROUND_LOADING
	typedef ThreadWorker GlLoadWorker;
#else
	typedef EventLoop GlLoadWorker;
#endif

	constexpr GLenum GlIndexTypeLookup[] = { GL_UNSIGNED_INT, GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };
	PE_STATIC_ASSERT(PE_ARRAY_LEN(GlIndexTypeLookup) == static_cast<int>(graphics::IndexType::ENUM_VALUE_COUNT), PE_TEXT("GlIndexTypeLookup array need to be updated"));
	constexpr GLenum GlMeshAttribTypeLookup[] = { GL_FLOAT, GL_HALF_FLOAT, GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT };
	PE_STATIC_ASSERT(PE_ARRAY_LEN(GlMeshAttribTypeLookup) == static_cast<int>(graphics::MeshAttribType::ENUM_VALUE_COUNT), PE_TEXT("GlMeshAttribTypeLookup array need to be updated"));
	constexpr GLenum GlCullFaceLookup[] = { GL_BACK, GL_FRONT, GL_FRONT_AND_BACK, GL_BACK };
	PE_STATIC_ASSERT(PE_ARRAY_LEN(GlCullFaceLookup) == static_cast<int>(graphics::CullMode::ENUM_VALUE_COUNT), PE_TEXT("GlCullFaceLookup array need to be updated"));

	struct GlContextWrapper {
		GladGLContext gl;
		SDL_GLContext opengl_context;
		WorkQueue& context_worker;
		GLuint vertex_array_object;
		bool init;

		GlContextWrapper(WorkQueue& worker) :
			context_worker(worker),
			init(false),
			opengl_context(nullptr),
			vertex_array_object(0) {
		}
	};
}
#endif // !PE_DISABLE_OPENGL
