#ifndef PE_DISABLE_OPENGL
#ifndef PE_GL_GRAPHICS_OPENGL_GRAPHICS_H_
#define PE_GL_GRAPHICS_OPENGL_GRAPHICS_H_
#include "PE_defines.h"
#include "PE_opengl_common.h"
#include "PE_opengl_window.h"
#include "PE_graphics_implementation.h"

namespace pe::internal::opengl {
	class OpenGlGraphics : public GraphicsImplementation {
	public:
		OpenGlGraphics();
		virtual ~OpenGlGraphics();
		PE_NOCOPY(OpenGlGraphics);
		PE_NODISCARD Window* CreateWindow(char* title, int width, int height, Uint32 flags, ErrorCode* err_out) PE_OVERRIDE;
		PE_NODISCARD graphics::Shader* LoadShader(const char* name, ErrorCode* err_out) PE_OVERRIDE;
		PE_NODISCARD graphics::RenderMesh* CreateRenderMesh(void* vertices, size_t vertices_size, void* indices, size_t indices_size, ErrorCode* err_out) PE_OVERRIDE;
		PE_NODISCARD graphics::CommandQueue* CreateCommandQueue(ErrorCode* err_out) PE_OVERRIDE;
		ErrorCode RenderToWindow(Window* target_window, graphics::CommandQueue** queues, size_t queue_count) PE_OVERRIDE;

	protected:
		
	};
}
#endif // !PE_GL_GRAPHICS_OPENGL_GRAPHICS_H_
#endif // !PE_DISABLE_OPENGL