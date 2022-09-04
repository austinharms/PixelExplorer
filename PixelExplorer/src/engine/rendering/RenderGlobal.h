#include <stdint.h>
#include <string>
#include <mutex>

#include "common/Logger.h"
#include "GL/glew.h"

#ifndef PIXELEXPLORE_ENGINE_RENDERING_RENDERGLOBAL_H_
#define PIXELEXPLORE_ENGINE_RENDERING_RENDERGLOBAL_H_

namespace pixelexplorer::engine::rendering::global {
	uint32_t windowCount = 0;
	bool glfwInit = false;
	std::mutex windowCreationLock;

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
