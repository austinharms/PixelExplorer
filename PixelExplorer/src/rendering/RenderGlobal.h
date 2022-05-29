#include <stdint.h>
#include <string>

#include "Logger.h"
#include "GL/glew.h"

#ifndef PIXELEXPLORE_RENDERING_RENDERGLOBAL_H_
#define PIXELEXPLORE_RENDERING_RENDERGLOBAL_H_

namespace pixelexplore::rendering::global {
	uint32_t windowCount = 0;
	bool glfwInit = false;

	void glfwErrorCallback(int error, const char* description) {
		Logger::error("GLFW Code: " + std::to_string(error) + " Description: " + description);
	}

	void GLAPIENTRY glErrorCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const char* message,
		const void* userParam) {
		if (type != 0x8251) {
			if (type == GL_DEBUG_TYPE_ERROR) {
				Logger::error("GL: type = " + std::to_string(type) +
					", severity = " + std::to_string(severity) +
					", message = " + message);
			}
			else {
				Logger::warn("GL: type = " + std::to_string(type) + ", severity = " +
					std::to_string(severity) + ", message = " + message);
			}
		}
	}
}

#endif // !PIXELEXPLORE_RENDERGLOBAL_H_
