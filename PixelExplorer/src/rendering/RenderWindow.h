#include <stdint.h>
#include <unordered_map>
#include <string>
#include <thread>
#include <forward_list>
#include <mutex>

#include "RefCount.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "RenderMesh.h"

#ifndef PIXELEXPLORE_RENDERING_RENDERWINDOW_H_
#define PIXELEXPLORE_RENDERING_RENDERWINDOW_H_
namespace pixelexplore::rendering {

	class RenderWindow : public RefCount
	{
	public:
		RenderWindow(int32_t width, int32_t height, const char* title);
		virtual ~RenderWindow();
		bool shouldClose() const;
		void drawFrame();
		Shader* loadShader(std::string path);
		bool dropShader(Shader* shader);
		void addRenderMesh(RenderMesh* renderMesh);
		void removeRenderMesh(RenderMesh* renderMesh);

	private:
		static void glfwStaticResizeCallback(GLFWwindow* window, int width, int height);
		static void glfwStaticFocusCallback(GLFWwindow* window, int focused);

		GLFWwindow* _window;
		std::thread::id _spawnThreadId;
		std::unordered_map<std::string, Shader*> _loadedShaders;
		std::forward_list<RenderMesh*> _addedRenderMeshes;
		std::forward_list<RenderMesh*> _removedRenderMeshes;
		std::forward_list<RenderMesh*> _renderMeshes;
		std::mutex _addRemoveRenderMeshMutex;

		void glfwResizeCallback(uint32_t width, uint32_t height);
		void glfwFocusCallback(bool focused);
	};
}
#endif // !PIXELEXPLORE_RENDERWINDOW_H_

