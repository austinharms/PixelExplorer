#include "opengl_graphics/PE_opengl_graphics.h"
#include "opengl_graphics/PE_opengl_window.h"

namespace pe::internal::opengl {
	OpenGlGraphics::OpenGlGraphics() {

	}

	OpenGlGraphics::~OpenGlGraphics() {

	}

	Window* CreateWindow(char* title, int width, int height, Uint32 flags) {
		return static_cast<Window*>(GlWindow::Create(title, width, height, flags));
	}
}