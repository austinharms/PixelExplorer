#ifndef PE_DISABLE_OPENGL
#ifndef PE_OPENGL_ADAPTER_H_
#define PE_OPENGL_ADAPTER_H_
#include "PE_defines.h"
#include "PE_graphics_implementation.h"

namespace pecore::graphics::implementation {
	PE_NODISCARD GraphicsCommands* PE_CALL AllocateOpenGlImplementation();
}
#endif // !PE_OPENGL_ADAPTER_H_
#endif // !PE_DISABLE_OPENGL
