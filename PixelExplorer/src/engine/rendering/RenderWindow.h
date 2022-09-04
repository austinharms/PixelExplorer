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
#include "CameraInterface.h"
#include "common/RefCount.h"
#include "RenderWindow.fwd.h"
#include "GLNode.h"
#include "GLObject.fwd.h"
#include "GLRenderObject.h"
#include "Shader.h"
#include "glm/mat4x4.hpp"

#ifndef PIXELEXPLORE_ENGINE_RENDERING_RENDERWINDOW_H_
#define PIXELEXPLORE_ENGINE_RENDERING_RENDERWINDOW_H_
namespace pixelexplorer::engine::rendering {
	class RenderWindow : public RefCount
	{
	public:
		RenderWindow(int32_t width, int32_t height, const char* title, CameraInterface* camera = nullptr);
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
		void setCamera(CameraInterface* camera);

		inline float getWindowWidth() const { return _windowWidth; }

		inline float getWindowHeight() const { return _windowHeight; }

		inline float getWindowScale() const { return _windowScale; }

		inline Shader* getActiveShader() const { return _activeShader; }

		inline const CameraInterface* getCamera() const { return _camera; }

	private:
		friend class GLObject;
		friend class Shader;
		static void glfwStaticResizeCallback(GLFWwindow* window, int width, int height);
		static void glfwStaticFocusCallback(GLFWwindow* window, int focused);

		GLFWwindow* _window;
		ImGuiContext* _guiContext;
		CameraInterface* _camera;
		Shader* _activeShader;
		GLNode<GLRenderObject> _glRenderObjects;
		GLNode<GLObject> _staticGLObjects;
		GLNode<GLObject> _staticGLObjectsRemoveQueue;
		std::unordered_map<std::string, Shader*> _loadedShaders;
		std::unordered_map<std::string, ImFont*> _loadedFonts;
		std::recursive_mutex _glRenderObjectsMutex;
		std::recursive_mutex _staticGLObjectsMutex;
		std::thread::id _spawnThreadId;
		uint32_t _windowWidth;
		uint32_t _windowHeight;
		float _windowScale;
		bool _activatedGuiContext;

		void glfwResizeCallback(uint32_t width, uint32_t height);
		void glfwFocusCallback(bool focused);
		bool terminateGLObject(GLObject* glObject);
		bool terminateGLObjectUnsafe(GLObject* glObject);
		void removeShaderFromCache(Shader* shader);
		void updateGLQueues();
		void drawRenderObjects();
	};
}
#endif // !PIXELEXPLORE_RENDERWINDOW_H_

