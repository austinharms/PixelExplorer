#include <stdint.h>
#include <unordered_map>
#include <string>
#include <thread>
#include <list>
#include <mutex>
#include <math.h>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "imgui.h"

#include "RefCount.h"
#include "RenderWindow.fwd.h"
#include "GLObject.fwd.h"
#include "BasicGLRenderObject.h"
#include "GLAsset.h"
#include "Shader.h"
#include "glm/mat4x4.hpp"

#ifndef PIXELEXPLORE_RENDERING_RENDERWINDOW_H_
#define PIXELEXPLORE_RENDERING_RENDERWINDOW_H_
namespace pixelexplorer::rendering {
	class RenderWindow : public RefCount
	{
	public:
		RenderWindow(int32_t width, int32_t height, const char* title);
		virtual ~RenderWindow();

		inline float getWindowWidth() const { return _windowWidth; }
		inline float getWindowHeight() const { return _windowHeight; }
		inline float getWindowScale() const { return fminf(_windowWidth / 600, _windowHeight / 400); }
		bool shouldClose() const;

		void drawFrame();
		Shader* getShader(std::string path);
		ImFont* getFont(const std::string& path);
		void addGLAsset(GLAsset* asset);
		void addGLRenderObject(BasicGLRenderObject* renderObject);
		void updateGLAsset(GLAsset* asset);

		void loadImGuiContext();
		void setShader(const Shader* shader);
		void setModelMatrix(const glm::mat4& mtx);

	private:
		friend class GLObject;
		friend class Shader;
		static void glfwStaticResizeCallback(GLFWwindow* window, int width, int height);
		static void glfwStaticFocusCallback(GLFWwindow* window, int focused);

		GLFWwindow* _window;
		ImGuiContext* _guiContext;
		Shader* _currentShader;
		float _windowWidth;
		float _windowHeight;
		std::thread::id _spawnThreadId;
		std::unordered_map<std::string, Shader*> _loadedShaders;
		std::unordered_map<std::string, ImFont*> _loadedFonts;
		GLObjectNode _glAssets;
		GLObjectNode _glRenderObjects;
		std::list<GLObject*> _glObjectAddQueue;
		std::list<GLObject*> _glObjectRemoveQueue;
		std::recursive_mutex _glRenderObjectsMutex;
		std::recursive_mutex _glAssetsMutex;
		std::recursive_mutex _glQueueMutex;
		glm::mat4 _viewMatrix;
		glm::mat4 _projectionMatrix;
		// the view and projection matrix multiplied together
		glm::mat4 _vpMatrix;
		// used to track if we need to render ImGui
		bool _loadedImGuiContext;

		void glfwResizeCallback(uint32_t width, uint32_t height);
		void glfwFocusCallback(bool focused);
		void removeGLObject(GLObject* glObject);
		void removeShader(Shader* shader);
		void updateGLQueues();
		void drawRenderObjects();
	};
}
#endif // !PIXELEXPLORE_RENDERWINDOW_H_

