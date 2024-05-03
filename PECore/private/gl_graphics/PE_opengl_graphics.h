#ifndef PE_DISABLE_OPENGL
#ifndef PE_GL_GRAPHICS_OPENGL_GRAPHICS_H_
#define PE_GL_GRAPHICS_OPENGL_GRAPHICS_H_
#include "PE_defines.h"
#include "PE_graphics_implementation.h"

namespace pecore::graphics {
	PE_NODISCARD GraphicsCommands* PE_CALL AllocateOpenGlImplementation();
}
#endif // !PE_GL_GRAPHICS_OPENGL_GRAPHICS_H_
#endif // !PE_DISABLE_OPENGL