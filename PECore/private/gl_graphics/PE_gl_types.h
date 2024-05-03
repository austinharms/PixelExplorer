#ifndef PE_DISABLE_OPENGL
#ifndef PE_GL_GRAPHICS_GL_TYPES_H_
#define PE_GL_GRAPHICS_GL_TYPES_H_
#include "PE_gl_defines.h"
#include "PE_graphics_types.h"
#include "PE_worker_thread.h"
#include "PE_event_loop.h"
#include "glad/gl.h"
#include "SDL_video.h"

namespace pecore::graphics::open_gl {
#if PE_GL_BACKGROUND_LOADING
	typedef ThreadWorker GlLoadWorker;
#else
	typedef pecore::EventLoopClass GlLoadWorker;
#endif

	constexpr GLenum GlIndexTypeLookup[] = { GL_UNSIGNED_INT, GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT };
	PE_STATIC_ASSERT(PE_ARRAY_LEN(GlIndexTypeLookup) == PE_INDEX_ENUM_VALUE_COUNT, PE_TEXT("GlIndexTypeLookup array need to be updated"));
	constexpr GLenum GlMeshAttribTypeLookup[] = { GL_FLOAT, GL_HALF_FLOAT, GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT };
	PE_STATIC_ASSERT(PE_ARRAY_LEN(GlMeshAttribTypeLookup) == PE_MESH_ATR_ENUM_VALUE_COUNT, PE_TEXT("GlMeshAttribTypeLookup array need to be updated"));
	constexpr GLenum GlCullFaceLookup[] = { GL_BACK, GL_FRONT, GL_FRONT_AND_BACK, GL_BACK };
	PE_STATIC_ASSERT(PE_ARRAY_LEN(GlCullFaceLookup) == PE_CULL_ENUM_VALUE_COUNT, PE_TEXT("GlCullFaceLookup array need to be updated"));

	struct GlContextWrapper {
		GladGLContext gl;
		SDL_GLContext opengl_context;
		GLuint vertex_array_object;
		bool init;

		GlContextWrapper() :
			init(false), 
			opengl_context(nullptr), 
			vertex_array_object(0) {
		}
	};
}
#endif // !PE_GL_GRAPHICS_GL_TYPES_H_
#endif // !PE_DISABLE_OPENGL
