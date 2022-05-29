#include <stdint.h>

#include "RefCount.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#ifndef PIXELEXPLORE_RENDERWINDOW_H_
#define PIXELEXPLORE_RENDERWINDOW_H_
namespace pixelexplore::rendering {

	class RenderWindow : public RefCount
	{
	public:
		RenderWindow(int32_t width, int32_t height, const char* title);
		virtual ~RenderWindow();
		bool shouldClose() const;
		void drawFrame();

	private:
		static void glfwStaticResizeCallback(GLFWwindow* window, int width, int height);
		static void glfwStaticFocusCallback(GLFWwindow* window, int focused);

		GLFWwindow* _window;

		void glfwResizeCallback(uint32_t width, uint32_t height);
		void glfwFocusCallback(bool focused);
	};
}
#endif // !PIXELEXPLORE_RENDERWINDOW_H_

