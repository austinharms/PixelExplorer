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
#include "GLNode.h"
#include "GLObject.fwd.h"
#include "GLRenderObject.h"
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
		bool shouldClose() const;
		void drawFrame();
		Shader* getShader(std::string path);
		ImFont* getFont(const std::string& path);
		void registerGLObject(GLObject* obj);
		void addGLRenderObject(GLRenderObject* renderObject);
		void removeGLRenderObject(GLRenderObject* renderObject);
		void loadImGuiContext();
		void setActiveShader(const Shader* shader);
		void setModelMatrix(const glm::mat4& mtx);

		inline float getWindowWidth() const { return _windowWidth; }

		inline float getWindowHeight() const { return _windowHeight; }

		inline float getWindowScale() const { return fminf(_windowWidth / 600, _windowHeight / 400); }

		inline Shader* getActiveShader() const { return _currentShader; }

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
		GLNode<GLRenderObject> _glRenderObjects;
		GLNode<GLObject> _staticGLObjects;
		GLNode<GLObject> _staticGLObjectsRemoveQueue;
		std::recursive_mutex _glRenderObjectsMutex;
		std::recursive_mutex _staticGLObjectsMutex;
		glm::mat4 _viewMatrix;
		glm::mat4 _projectionMatrix;
		// the view and projection matrix multiplied together
		glm::mat4 _vpMatrix;
		// used to track if we need to render ImGui
		bool _loadedImGuiContext;

		void glfwResizeCallback(uint32_t width, uint32_t height);
		void glfwFocusCallback(bool focused);
		void terminateGLObject(GLObject* glObject);
		void terminateGLObjectUnsafe(GLObject* glObject);
		void removeShaderFromCache(Shader* shader);
		void updateGLQueues();
		void drawRenderObjects();
	};
}
#endif // !PIXELEXPLORE_RENDERWINDOW_H_

