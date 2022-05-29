#include "RenderWindow.h"

#include <string>

#include "RenderGlobal.h"
#include "Logger.h"

namespace pixelexplore::rendering {
	RenderWindow::RenderWindow(int32_t width, int32_t height, const char* title)
	{
		glfwSetErrorCallback(global::glfwErrorCallback);
		global::glfwInit = glfwInit();
		if (!global::glfwInit) {
			const char* error;
			glfwGetError(&error);
			Logger::error(error);
			Logger::fatal("Failed to Init GLFW");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!_window)
		{
			const char* error;
			glfwGetError(&error);
			Logger::error(error);
			Logger::fatal("Failed to Create Window");
		}

		++global::windowCount;
		glfwSetWindowUserPointer(_window, this);
		glfwMakeContextCurrent(_window);
		glfwSetWindowSizeCallback(_window, glfwStaticResizeCallback);
		glfwSetWindowFocusCallback(_window, glfwStaticFocusCallback);

		GLenum initCode = glewInit();
		if (initCode != GLEW_OK) {
			Logger::error(std::to_string(initCode) + ": " + (const char*)glewGetErrorString(initCode));
			Logger::fatal("Failed to Init GLEW");
		}

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(global::glErrorCallback, 0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	RenderWindow::~RenderWindow()
	{
		glfwDestroyWindow(_window);
		if (--global::windowCount == 0) {
			glfwTerminate();
			global::glfwInit = false;
		}

		Logger::debug("Window closed");
	}

	bool RenderWindow::shouldClose() const
	{
		return glfwWindowShouldClose(_window);
	}

	void RenderWindow::drawFrame()
	{
		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	void RenderWindow::glfwStaticResizeCallback(GLFWwindow* window, int width, int height)
	{
		((RenderWindow*)glfwGetWindowUserPointer(window))->glfwResizeCallback(width, height);
	}

	void RenderWindow::glfwStaticFocusCallback(GLFWwindow* window, int focused)
	{
		((RenderWindow*)glfwGetWindowUserPointer(window))->glfwFocusCallback(focused);
	}

	void RenderWindow::glfwResizeCallback(uint32_t width, uint32_t height)
	{
		Logger::debug("Window resized: " + std::to_string(width) + "X" + std::to_string(height));
	}

	void RenderWindow::glfwFocusCallback(bool focused)
	{
		Logger::debug("Window focused: " + std::to_string(focused));
	}
}